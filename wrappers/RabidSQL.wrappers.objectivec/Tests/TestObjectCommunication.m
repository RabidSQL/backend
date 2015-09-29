#import <XCTest/XCTest.h>

#include "application.h"
#include "smartobject.h"
#include "variant.h"

@interface SmartObjectTester : SmartObject

@property int lastId;
@property NSMutableArray *lastArguments;

@end

@interface TestObjectCommunication : XCTestCase
@end

@implementation SmartObjectTester

- (id) init
{
    self = [super init];

    if (self) {
        _lastId = -1;
        _lastArguments = 0;
    }

    return self;
}

- (void) processQueueItem: (int) id withArguments: (NSMutableArray *) arguments
{
    _lastId = id;
    _lastArguments = arguments;
}

@end

@implementation TestObjectCommunication

- (void)testSendMessage {

    SmartObjectTester *sender = [[SmartObjectTester alloc] init];
    SmartObjectTester *receiver = [[SmartObjectTester alloc] init];

    [sender connectQueue: 1 withReceiver:receiver];
    NSMutableArray *arguments = [[NSMutableArray alloc] init];
    [arguments addObject: [[Variant alloc] initWithString: @"test"]];
    [sender queueData: 1 withArguments: arguments];

    XCTAssertEqual(receiver.lastId, -1);

    [Application processEvents];

    XCTAssertEqual(receiver.lastId, 1);
    XCTAssertEqualObjects(receiver.lastArguments, arguments);
}

@end
