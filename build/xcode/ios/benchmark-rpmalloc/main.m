//
//  main.m
//  benchmark-rpmalloc
//
//  Created by Mattias Jansson on 2017-04-05.
//
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"

static int main_argc;
static char** main_argv;

int
benchmark_run(int argc, char** argv);

@interface MainThread :
NSObject
+ (void)startMainThread:(void*)arg;
@end

@implementation MainThread

+ (void)startMainThread:(void*)arg {
	(void)sizeof(arg);
	benchmark_run(main_argc, main_argv);
}

@end

int main(int argc, char * argv[]) {
	main_argc = argc;
	main_argv = argv;
	@autoreleasepool {
		[NSThread detachNewThreadSelector:@selector(startMainThread:) toTarget:[MainThread class] withObject:nil];
	    return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
	}
}
