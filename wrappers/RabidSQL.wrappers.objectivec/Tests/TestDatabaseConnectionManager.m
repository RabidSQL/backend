#import <XCTest/XCTest.h>

// @TODO: Change all MySQL-specific tests that aren't testing specific
// functionality to use SQLite once it's implemented

#include "application.h"
#include "connectionsettings.h"
#include "databaseconnection.h"
#include "databaseconnectionfactory.h"
#include "databaseconnectionmanager.h"
#include "structures.h"
#include "variant.h"

@interface TestDatabaseConnectionManager : XCTestCase

@end

@interface DatabaseConnectionReceiver : SmartObject

@property int lastId;
@property NSMutableArray *lastArguments;

@end

@implementation DatabaseConnectionReceiver

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

@implementation TestDatabaseConnectionManager

- (void) testConnectionManager {
    ConnectionSettings *settings = [[ConnectionSettings alloc] init];
    DatabaseConnectionManager *manager;
    QueryResult *result;
    DatabaseConnectionReceiver *receiver;
    Variant *variant = [[Variant alloc] init];

    // Configure connection settings
    [variant setNumber: [NSNumber numberWithInt: MYSQL] withType: D_UINT];
    [variant setString: @"localhost"];
    [settings set: @"hostname" withValue: variant];
    [variant setString: @"test"];
    [settings set: @"username" withValue: variant];

    // Make connection manager
    manager = [DatabaseConnectionFactory makeManager: settings];

    receiver = [[DatabaseConnectionReceiver alloc] init];

    NSString *uuid = [manager reserveDatabaseConnection: 0
                                           withReceiver: receiver];
    NSMutableArray *arguments = [[NSMutableArray alloc] init];

    [variant setString: @"uid"];
    [manager blockingCall: uuid
                  withUid: variant
                  ofEvent: LIST_DATABASES
            withArguments: arguments];

    [Application processEvents];

    XCTAssertEqual(receiver.lastId, [DatabaseConnection getExecutedId]);

    XCTAssertEqual(receiver.lastArguments.count, 3);

    variant = receiver.lastArguments[0];
    XCTAssertEqualObjects([variant toString], @"uid");

    variant = receiver.lastArguments[1];
    XCTAssertEqual([[variant toNumber] unsignedIntegerValue],
                   (int) LIST_DATABASES);

    XCTAssertFalse(result.error.isError);

    variant = receiver.lastArguments[2];
    result = [variant toQueryResult];
    XCTAssertGreaterThan(result.rows.count, 0);

}

@end
