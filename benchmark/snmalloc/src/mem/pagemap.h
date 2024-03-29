#pragma once

#include "../ds/bits.h"
#include "../ds/helpers.h"
#include "../ds/invalidptr.h"

#include <atomic>
#include <utility>

namespace snmalloc
{
  static constexpr size_t PAGEMAP_NODE_BITS = 16;
  static constexpr size_t PAGEMAP_NODE_SIZE = 1ULL << PAGEMAP_NODE_BITS;

  /**
   * Structure describing the configuration of a pagemap.  When querying a
   * pagemap from a different instantiation of snmalloc, the pagemap is exposed
   * as a `void*`.  This structure allows the caller to check whether the
   * pagemap is of the format that they expect.
   */
  struct PagemapConfig
  {
    /**
     * The version of the pagemap structure.  This is always 1 in existing
     * versions of snmalloc.  This will be incremented every time the format
     * changes in an incompatible way.  Changes to the format may add fields to
     * the end of this structure.
     */
    uint32_t version;
    /**
     * Is this a flat pagemap?  If this field is false, the pagemap is the
     * hierarchical structure.
     */
    bool is_flat_pagemap;
    /**
     * Number of bytes in a pointer.
     */
    uint8_t sizeof_pointer;
    /**
     * The number of bits of the address used to index into the pagemap.
     */
    uint64_t pagemap_bits;
    /**
     * The size (in bytes) of a pagemap entry.
     */
    size_t size_of_entry;
  };

  /**
   * The Pagemap is the shared data structure ultimately used by multiple
   * snmalloc threads / allocators to determine who owns memory and,
   * therefore, to whom deallocated memory should be returned.  The
   * allocators do not interact with this directly but rather via the
   * static ChunkMap object, which encapsulates knowledge about the
   * pagemap's parametric type T.
   *
   * The other template paramters are...
   *
   *   GRANULARITY_BITS: the log2 of the size in bytes of the address space
   *   granule associated with each entry.
   *
   *   default_content: An initial value of T (typically "0" or something akin)
   *
   *   PrimAlloc: A class used to source PageMap-internal memory; it must have a
   *   method callable as if it had the following type:
   *
   *      template<typename T, size_t alignment> static T* alloc_chunk(void);
   */
  template<
    size_t GRANULARITY_BITS,
    typename T,
    T default_content,
    typename PrimAlloc>
  class Pagemap
  {
  private:
    static constexpr size_t COVERED_BITS =
      bits::ADDRESS_BITS - GRANULARITY_BITS;
    static constexpr size_t CONTENT_BITS =
      bits::next_pow2_bits_const(sizeof(T));

    static_assert(
      PAGEMAP_NODE_BITS - CONTENT_BITS < COVERED_BITS,
      "Should use the FlatPageMap as it does not require a tree");

    static constexpr size_t BITS_FOR_LEAF = PAGEMAP_NODE_BITS - CONTENT_BITS;
    static constexpr size_t ENTRIES_PER_LEAF = 1 << BITS_FOR_LEAF;
    static constexpr size_t LEAF_MASK = ENTRIES_PER_LEAF - 1;

    static constexpr size_t BITS_PER_INDEX_LEVEL =
      PAGEMAP_NODE_BITS - POINTER_BITS;
    static constexpr size_t ENTRIES_PER_INDEX_LEVEL = 1 << BITS_PER_INDEX_LEVEL;
    static constexpr size_t ENTRIES_MASK = ENTRIES_PER_INDEX_LEVEL - 1;

    static constexpr size_t INDEX_BITS =
      BITS_FOR_LEAF > COVERED_BITS ? 0 : COVERED_BITS - BITS_FOR_LEAF;

    static constexpr size_t INDEX_LEVELS = INDEX_BITS / BITS_PER_INDEX_LEVEL;
    static constexpr size_t TOPLEVEL_BITS =
      INDEX_BITS - (INDEX_LEVELS * BITS_PER_INDEX_LEVEL);
    static constexpr size_t TOPLEVEL_ENTRIES = 1 << TOPLEVEL_BITS;
    static constexpr size_t TOPLEVEL_SHIFT =
      (INDEX_LEVELS * BITS_PER_INDEX_LEVEL) + BITS_FOR_LEAF + GRANULARITY_BITS;

    // Value used to represent when a node is being added too
    static constexpr InvalidPointer<1> LOCKED_ENTRY{};

    struct Leaf
    {
      TrivialInitAtomic<T> values[ENTRIES_PER_LEAF];

      static_assert(sizeof(TrivialInitAtomic<T>) == sizeof(T));
      static_assert(alignof(TrivialInitAtomic<T>) == alignof(T));
    };

    struct PagemapEntry
    {
      TrivialInitAtomic<PagemapEntry*> entries[ENTRIES_PER_INDEX_LEVEL];

      static_assert(
        sizeof(TrivialInitAtomic<PagemapEntry*>) == sizeof(PagemapEntry*));
      static_assert(
        alignof(TrivialInitAtomic<PagemapEntry*>) == alignof(PagemapEntry*));
    };

    static_assert(
      sizeof(PagemapEntry) == sizeof(Leaf), "Should be the same size");

    static_assert(
      sizeof(PagemapEntry) == PAGEMAP_NODE_SIZE, "Should be the same size");

    // Init removed as not required as this is only ever a global
    // cl is generating a memset of zero, which will be a problem
    // in libc/ucrt bring up.  On ucrt this will run after the first
    // allocation.
    //  TODO: This is fragile that it is not being memset, and we should review
    //  to ensure we don't get bitten by this in the future.
    TrivialInitAtomic<PagemapEntry*> top[TOPLEVEL_ENTRIES];

    template<bool create_addr>
    SNMALLOC_FAST_PATH PagemapEntry*
    get_node(TrivialInitAtomic<PagemapEntry*>* e, bool& result)
    {
      // The page map nodes are all allocated directly from the OS zero
      // initialised with a system call.  We don't need any ordered to guarantee
      // to see that correctly. The only transistions are monotone and handled
      // by the slow path.
      PagemapEntry* value = e->load(std::memory_order_relaxed);

      if (likely(value > LOCKED_ENTRY))
      {
        result = true;
        return value;
      }
      if constexpr (create_addr)
      {
        return get_node_slow(e, result);
      }
      else
      {
        result = false;
        return nullptr;
      }
    }

    SNMALLOC_SLOW_PATH PagemapEntry*
    get_node_slow(TrivialInitAtomic<PagemapEntry*>* e, bool& result)
    {
      // The page map nodes are all allocated directly from the OS zero
      // initialised with a system call.  We don't need any ordered to guarantee
      // to see that correctly.
      PagemapEntry* value = e->load(std::memory_order_relaxed);

      if ((value == nullptr) || (value == LOCKED_ENTRY))
      {
        value = nullptr;

        if (e->compare_exchange_strong(
              value, LOCKED_ENTRY, std::memory_order_relaxed))
        {
          value = PrimAlloc::template alloc_chunk<PagemapEntry, OS_PAGE_SIZE>();
          e->store(value, std::memory_order_release);
        }
        else
        {
          while (address_cast(e->load(std::memory_order_relaxed)) ==
                 LOCKED_ENTRY)
          {
            Aal::pause();
          }
          value = e->load(std::memory_order_acquire);
        }
      }
      result = true;
      return value;
    }

    template<bool create_addr>
    SNMALLOC_FAST_PATH std::pair<Leaf*, size_t>
    get_leaf_index(uintptr_t addr, bool& result)
    {
#ifdef FreeBSD_KERNEL
      // Zero the top 16 bits - kernel addresses all have them set, but the
      // data structure assumes that they're zero.
      addr &= 0xffffffffffffULL;
#endif
      size_t ix = addr >> TOPLEVEL_SHIFT;
      size_t shift = TOPLEVEL_SHIFT;
      TrivialInitAtomic<PagemapEntry*>* e = &top[ix];

      // This is effectively a
      //   for (size_t i = 0; i < INDEX_LEVELS; i++)
      // loop, but uses constexpr to guarantee optimised version
      // where the INDEX_LEVELS in {0,1}.
      if constexpr (INDEX_LEVELS != 0)
      {
        size_t i = 0;
        while (true)
        {
          PagemapEntry* value = get_node<create_addr>(e, result);
          if (unlikely(!result))
            return {nullptr, 0};

          shift -= BITS_PER_INDEX_LEVEL;
          ix = (static_cast<size_t>(addr) >> shift) & ENTRIES_MASK;
          e = &value->entries[ix];

          if constexpr (INDEX_LEVELS == 1)
          {
            UNUSED(i);
            break;
          }
          else
          {
            i++;
            if (i == INDEX_LEVELS)
              break;
          }
        }
      }

      Leaf* leaf = reinterpret_cast<Leaf*>(get_node<create_addr>(e, result));

      if (unlikely(!result))
        return {nullptr, 0};

      shift -= BITS_FOR_LEAF;
      ix = (static_cast<size_t>(addr) >> shift) & LEAF_MASK;
      return {leaf, ix};
    }

    template<bool create_addr>
    SNMALLOC_FAST_PATH TrivialInitAtomic<T>*
    get_addr(uintptr_t p, bool& success)
    {
      auto leaf_ix = get_leaf_index<create_addr>(p, success);
      return &(leaf_ix.first->values[leaf_ix.second]);
    }

    TrivialInitAtomic<T>* get_ptr(uintptr_t p)
    {
      bool success;
      return get_addr<true>(p, success);
    }

  public:
    /**
     * The pagemap configuration describing this instantiation of the template.
     */
    static constexpr PagemapConfig config = {
      1, false, sizeof(uintptr_t), GRANULARITY_BITS, sizeof(T)};

    /**
     * Cast a `void*` to a pointer to this template instantiation, given a
     * config describing the configuration.  Return null if the configuration
     * passed does not correspond to this template instantiation.
     *
     * This intended to allow code that depends on the pagemap having a
     * specific representation to fail gracefully.
     */
    static Pagemap* cast_to_pagemap(void* pm, const PagemapConfig* c)
    {
      if (
        (c->version != 1) || (c->is_flat_pagemap) ||
        (c->sizeof_pointer != sizeof(uintptr_t)) ||
        (c->pagemap_bits != GRANULARITY_BITS) ||
        (c->size_of_entry != sizeof(T)) || (!std::is_integral_v<T>))
      {
        return nullptr;
      }
      return static_cast<Pagemap*>(pm);
    }

    /**
     * Returns the index of a pagemap entry within a given page.  This is used
     * in code that propagates changes to the pagemap elsewhere.
     */
    size_t index_for_address(uintptr_t p)
    {
      bool success;
      return (OS_PAGE_SIZE - 1) &
        reinterpret_cast<size_t>(get_addr<true>(p, success));
    }

    /**
     * Returns the address of the page containing
     */
    void* page_for_address(uintptr_t p)
    {
      bool success;
      return pointer_align_down<OS_PAGE_SIZE>(get_addr<true>(p, success));
    }

    T get(uintptr_t p)
    {
      bool success;
      auto addr = get_addr<false>(p, success);
      if (!success)
        return default_content;
      return addr->load(std::memory_order_relaxed);
    }

    void set(uintptr_t p, T x)
    {
      bool success;
      auto addr = get_addr<true>(p, success);
      addr->store(x, std::memory_order_relaxed);
    }

    void set_range(uintptr_t p, T x, size_t length)
    {
      bool success;
      do
      {
        auto leaf_ix = get_leaf_index<true>(p, success);
        size_t ix = leaf_ix.second;

        auto last = bits::min(LEAF_MASK + 1, ix + length);

        auto diff = last - ix;

        for (; ix < last; ix++)
        {
          SNMALLOC_ASSUME(leaf_ix.first != nullptr);
          leaf_ix.first->values[ix].store(x);
        }

        length = length - diff;
        p = p + (diff << GRANULARITY_BITS);
      } while (length > 0);
    }
  };

  /**
   * Simple pagemap that for each GRANULARITY_BITS of the address range
   * stores a T.
   */
  template<size_t GRANULARITY_BITS, typename T>
  class alignas(OS_PAGE_SIZE) FlatPagemap
  {
  private:
    static constexpr size_t COVERED_BITS =
      bits::ADDRESS_BITS - GRANULARITY_BITS;
    static constexpr size_t ENTRIES = 1ULL << COVERED_BITS;
    static constexpr size_t SHIFT = GRANULARITY_BITS;

    TrivialInitAtomic<T> top[ENTRIES];

    static_assert(sizeof(TrivialInitAtomic<T>) == sizeof(T));
    static_assert(alignof(TrivialInitAtomic<T>) == alignof(T));

  public:
    /**
     * The pagemap configuration describing this instantiation of the template.
     */
    static constexpr PagemapConfig config = {
      1, true, sizeof(uintptr_t), GRANULARITY_BITS, sizeof(T)};

    /**
     * Cast a `void*` to a pointer to this template instantiation, given a
     * config describing the configuration.  Return null if the configuration
     * passed does not correspond to this template instantiation.
     *
     * This intended to allow code that depends on the pagemap having a
     * specific representation to fail gracefully.
     */
    static FlatPagemap* cast_to_pagemap(void* pm, const PagemapConfig* c)
    {
      if (
        (c->version != 1) || (!c->is_flat_pagemap) ||
        (c->sizeof_pointer != sizeof(uintptr_t)) ||
        (c->pagemap_bits != GRANULARITY_BITS) ||
        (c->size_of_entry != sizeof(T)) || (!std::is_integral_v<T>))
      {
        return nullptr;
      }
      return static_cast<FlatPagemap*>(pm);
    }

    T get(uintptr_t p)
    {
      return top[p >> SHIFT].load(std::memory_order_relaxed);
    }

    void set(uintptr_t p, T x)
    {
      top[p >> SHIFT].store(x, std::memory_order_relaxed);
    }

    void set_range(uintptr_t p, T x, size_t length)
    {
      size_t index = p >> SHIFT;
      do
      {
        top[index].store(x, std::memory_order_relaxed);
        index++;
        length--;
      } while (length > 0);
    }

    /**
     * Returns the index within a page for the specified address.
     */
    size_t index_for_address(uintptr_t p)
    {
      return (static_cast<size_t>(p) >> SHIFT) % OS_PAGE_SIZE;
    }

    /**
     * Returns the address of the page containing the pagemap address p.
     */
    void* page_for_address(uintptr_t p)
    {
      SNMALLOC_ASSERT(
        (reinterpret_cast<uintptr_t>(&top) & (OS_PAGE_SIZE - 1)) == 0);
      return reinterpret_cast<void*>(
        reinterpret_cast<uintptr_t>(&top[p >> SHIFT]) & ~(OS_PAGE_SIZE - 1));
    }
  };

  /**
   * Mixin used by `ChunkMap` and other `PageMap` consumers to directly access
   * the pagemap via a global variable.  This should be used from within the
   * library or program that owns the pagemap.
   *
   * This class makes the global pagemap a static field so that its name
   * includes the type mangling.  If two compilation units try to instantiate
   * two different types of pagemap then they will see two distinct pagemaps.
   * This will prevent allocating with one and freeing with the other (because
   * the memory will show up as not owned by any allocator in the other
   * compilation unit) but will prevent the same memory being interpreted as
   * having two different types.
   *
   * Simiarly, perhaps two modules wish to instantiate *different* pagemaps
   * of the *same* type.  Therefore, we add a `Purpose` parameter that can be
   * used to pry symbols apart.  By default, the `Purpose` is just the type of
   * the pagemap; that is, pagemaps default to discrimination solely by their
   * type.
   */
  template<typename T, typename Purpose = T>
  class GlobalPagemapTemplate
  {
    /**
     * The global pagemap variable.  The name of this symbol will include the
     * type of `T` and `U`.
     */
    SNMALLOC_FORCE_BSS
    inline static T global_pagemap;

  public:
    /**
     * Returns the pagemap.
     */
    static T& pagemap()
    {
      return global_pagemap;
    }
  };

  /**
   * Mixin used by `ChunkMap` and other `PageMap` consumers to access the global
   * pagemap via a type-checked C interface.  This should be used when another
   * library (e.g.  your C standard library) uses snmalloc and you wish to use a
   * different configuration in your program or library, but wish to share a
   * pagemap so that either version can deallocate memory.
   *
   * The `Purpose` parameter is as with `GlobalPgemapTemplate`.
   */
  template<
    typename T,
    void* (*raw_get)(const PagemapConfig**),
    typename Purpose = T>
  class ExternalGlobalPagemapTemplate
  {
    /**
     * A pointer to the pagemap.
     */
    inline static T* external_pagemap;

  public:
    /**
     * Returns the exported pagemap.
     * Accesses the pagemap via the C ABI accessor and casts it to
     * the expected type, failing in cases of ABI mismatch.
     */
    static T& pagemap()
    {
      if (external_pagemap == nullptr)
      {
        const snmalloc::PagemapConfig* c = nullptr;
        void* raw_pagemap = raw_get(&c);
        external_pagemap = T::cast_to_pagemap(raw_pagemap, c);
        if (!external_pagemap)
        {
          Pal::error("Incorrect ABI of global pagemap.");
        }
      }
      return *external_pagemap;
    }
  };

} // namespace snmalloc
