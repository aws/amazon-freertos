from attributesCommon import attributesCommon
import testutils

class gattClient(attributesCommon):
    services = {}
    characteristics = {}
    descriptors = {}

    @staticmethod
    def updateLocalAttributeTable():
        services = testutils.find_gatt_service_in_objects(gattClient.services, gattClient.characteristics, gattClient.descriptors )

