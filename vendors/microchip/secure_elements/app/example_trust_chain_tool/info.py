"""
Device Info Retrieval Example
"""
# (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
#
# Subject to your compliance with these terms, you may use Microchip software
# and any derivatives exclusively with Microchip products. It is your
# responsibility to comply with third party license terms applicable to your
# use of third party software (including open source software) that may
# accompany Microchip software.
#
# THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
# EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
# WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
# PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
# SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
# OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
# MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
# FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
# LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
# THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
# THIS SOFTWARE.

from cryptoauthlib import *
from cryptoauthlib.device import Atecc608aConfig
from common import *


def info(iface='hid', device='ecc', **kwargs):
    ATCA_SUCCESS = 0x00

    # Get the target default config
    cfg = eval('cfg_at{}a_{}_default()'.format(atca_names_map.get(device), atca_names_map.get(iface)))

    # Set interface parameters
    if kwargs is not None:
        for k, v in kwargs.items():
            icfg = getattr(cfg.cfg, 'atca{}'.format(iface))
            setattr(icfg, k, int(v, 16))

    # Basic Raspberry Pi I2C check
    if 'i2c' == iface and check_if_rpi():
        cfg.cfg.atcai2c.bus = 1

    # Initialize the stack
    assert atcab_init(cfg) == ATCA_SUCCESS
    print('')

    # Request the Revision Number
    info = bytearray(4)
    assert atcab_info(info) == ATCA_SUCCESS
    print('\nDevice Part:')
    print('    ' + get_device_name(info))

    # Request the Serial Number
    serial_number = bytearray(9)
    assert atcab_read_serial_number(serial_number) == ATCA_SUCCESS
    print('\nSerial number: ')
    print(pretty_print_hex(serial_number, indent='    '))

    # Read the configuration zone
    config_zone = bytearray(128)
    assert atcab_read_config_zone(config_zone) == ATCA_SUCCESS
    config = Atecc608aConfig.from_buffer(config_zone)

    print('\nConfiguration Zone:')
    print(pretty_print_hex(config_zone, indent='    '))

    # Check the device locks
    print('\nCheck Device Locks')
    is_locked = AtcaReference(False)
    assert atcab_is_locked(0, is_locked) == ATCA_SUCCESS
    config_zone_locked = bool(is_locked.value)
    print('    Config Zone is %s' % ('locked' if config_zone_locked else 'unlocked'))

    assert atcab_is_locked(1, is_locked) == ATCA_SUCCESS
    data_zone_locked = bool(is_locked.value)
    print('    Data Zone is %s' % ('locked' if data_zone_locked else 'unlocked'))

    # Load the public key
    if data_zone_locked:
        print('\nLoading Public key:')

        public_key = bytearray(64)
        assert atcab_get_pubkey(0, public_key) == ATCA_SUCCESS
        print(convert_ec_pub_to_pem(public_key))

    # Free the library
    atcab_release()


if __name__ == '__main__':
    parser = setup_example_runner(__file__)
    args = parser.parse_args()

    info(args.iface, args.device, **parse_interface_params(args.params))
    print('\nDone')
