#import <XCTest/XCTest.h>

#include "application.h"
#include "object.h"
#include "variant.h"
#include "logic/include/application.h"
#include "logic/include/structs.h"
#include "logic/include/variant.h"

// These test cases are not intended to test full functionality but simply that
// the wrapped backend classes are usable and contain acceptable data.

@interface TestSimpleWrappers : XCTestCase

@end

struct RVariant
{
    RabidSQL::Variant object;
};

@implementation TestSimpleWrappers

- (void) testApplicationWrapper {

    // Post a message
    RabidSQL::Application::postMessage(
        static_cast<RabidSQL::MessageType>(WARNING), "Label",
        RabidSQL::Variant("Data"));

    Message *message = [Application getNextMessage];

    XCTAssertEqual(message.type, WARNING);
    XCTAssertEqualObjects(message.label, @"Label");
    XCTAssertEqualObjects([message.data toString], @"Data");

    message = [Application getNextMessage];
    XCTAssertEqual(message.type, NO_MESSAGE);
}

- (void) testConnectionSettingsWrapper {
    ConnectionSettings *settings = [[ConnectionSettings alloc] init];

    NSString *key = @"Test";
    NSNumber *number = [NSNumber numberWithUnsignedInteger: 123];
    Variant *value = [[Variant alloc] initWithNumber:number withType: D_UINT];
    [settings set: key withValue: value];

    XCTAssert([settings contains: key]);
    XCTAssertEqualObjects([[settings get: key] toNumber], number);
}

- (void) testVariantWrapper {

    // Initialize core Variant
    RabidSQL::Variant rVariant(123);

    // Initialize Objective-C variant
    Variant *variant = Object::wrap(rVariant);

    XCTAssertEqual([[variant toNumber] unsignedIntegerValue], 123);

    // Re-assign
    rVariant = 456;

    // Ensure the OC version is a copy (i.e. the above change didn't change it)
    XCTAssertEqual([[variant toNumber] unsignedIntegerValue], 123);
}

@end
