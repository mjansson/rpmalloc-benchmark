/*
 *   Copyright (C) 2009, 2010, 2011 Lockless Inc., Steven Von Fuerst.
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <new>
#include "windows.h"
#include "errno.h"



/* Hack to use tls */
__declspec(thread) void *ll_hack_tls;

extern "C"
{
	int llalloc_use = 0;
	void lldebug_hook(void);
	void *valloc(size_t size);
	int handle_oom(int size);

	void llmutex_lock(SRWLOCK *l);
	void llmutex_unlock(SRWLOCK *l);
	int llmutex_trylock(SRWLOCK *l);
};

/* Replacement c++ memory operators for static compile */

void *operator new(size_t size)
{
	void *p = malloc(size);
	if (!p) throw std::bad_alloc();
	
	return p;
}

void *operator new[](size_t size)
{
	void *p = malloc(size);
	if (!p) throw std::bad_alloc();
	
	return p;
}

__declspec(nothrow) void *operator new(size_t size, const std::nothrow_t&)
{
	return malloc(size);
}

__declspec(nothrow) void *operator new[](size_t size, const std::nothrow_t&)
{
	return malloc(size);
}

__declspec(nothrow) void operator delete(void* p)
{
	free(p);
}

__declspec(nothrow) void operator delete[](void* p)
{
	free(p);
}

__declspec(nothrow) void operator delete(void* p, const std::nothrow_t&)
{
	free(p);
}

__declspec(nothrow) void operator delete[](void* p, const std::nothrow_t&)
{
	free(p);
}

#ifdef USE_DLL

extern "C"
{
	/* Hack - indirect calls to crt functions */
	extern int (* __callnewh)(size_t);
	extern int (* __newmode)(void);
}
#define _newmode (__newmode())
#define _callnewh __callnewh

#else /* USE_DLL */
extern "C"
{
	/* Undocumented functions that need declarations */
	int _callnewh(size_t size);
	extern int _newmode;
}
#endif /* USE_DLL */

int handle_oom(int size)
{
	if (_newmode)
	{
		__try
		{
			if (_callnewh(size)) return 1;
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			/* Do nothing, and fail with ENOMEM below */
		}
	}
	
	_set_errno(ENOMEM);
	return 0;
}

void lldebug_hook(void)
{
	void *a = valloc(0);
	free(a);
}

void llmutex_lock(SRWLOCK *l)
{
	AcquireSRWLockExclusive(l);
}

void llmutex_unlock(SRWLOCK *l)
{
	ReleaseSRWLockExclusive(l);
}

int llmutex_trylock(SRWLOCK *l)
{
	/* Quick check for users */
	if (*(void **) l) return EBUSY;
	
	/* Try to grab lock if it has no users */
	if (!InterlockedCompareExchangePointer((void **) l, (void *) 1, NULL)) return 0;
	
	return EBUSY;
}

