#include "unity_fixture.h"

TEST_GROUP( Full_CBOR );
TEST_GROUP_RUNNER( Full_CBOR )
{
    RUN_TEST_GROUP( aws_cbor_acceptance )
}
