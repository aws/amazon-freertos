using System;
using System.Collections.Generic;
using System.Data;
using System.Windows.Forms;
using System.IO;
using System.Security.Cryptography;
using System.Diagnostics;
using System.Linq;
using System.Collections;
using Renesas_Secure_Flash_Programmer.Properties;
using Org.BouncyCastle.Crypto;
using Org.BouncyCastle.OpenSsl;
using Org.BouncyCastle.Crypto.Signers;
using Org.BouncyCastle.Crypto.Parameters;
using Org.BouncyCastle.Crypto.Digests;
using Org.BouncyCastle.Math;
using System.Text;

namespace Renesas_Secure_Flash_Programmer
{
    public partial class FormMain : Form
    {
        /// <summary>
        /// Endian enum
        /// </summary>
        public enum Endian
        {
            Little,
            Big,
        }

        /// <summary>
        /// Mcu enum
        /// </summary>
        public enum Mcu
        {
            RX231,
            RX65N,
            RX66T,
            RX72T,
        }

        /// <summary>
        /// TSIP function level
        /// </summary>
        public enum TSIPLevel
        {
            Lite,
            Full,
        }

        /// <summary>
        /// Key Type enum
        /// </summary>
        public enum KeyType : int
        {
            AES128bit = 0,
            AES256bit,
            RSA1024bit_Public,
            RSA1024bit_Private,
            RSA2048bit_Public,
            RSA2048bit_Private,
            DES,
            DES2Key,
            TripleDES,
            UpdateKeyRing,
        }

        /// <summary>
        /// KeyType/KeyName string/KeyData length information struct
        /// </summary>
        public class KeyInfo
        {
            public KeyType Type;
            public string Name;
            public int DataLength;

            public KeyInfo(KeyType type, string name, int datalength)
            {
                Type = type;
                Name = name;
                DataLength = datalength;
            }

            public override string ToString()
            {
                return Name;
            }
        }

        /// <summary>
        /// KeyInfo list
        /// </summary>
        static readonly List<KeyInfo> KeyInfoList_Full = new List<KeyInfo>()
        {
            new KeyInfo( KeyType.AES128bit,            "AES-128bit",          16),
            new KeyInfo( KeyType.AES256bit,            "AES-256bit",          32),
            new KeyInfo( KeyType.RSA1024bit_Public,    "RSA-1024bit Public",  132), // key n : 128byte + key e 4byyte
            new KeyInfo( KeyType.RSA1024bit_Private,   "RSA-1024bit Private", 256), // key n : 128byte + key d 128byte
            new KeyInfo( KeyType.RSA2048bit_Public,    "RSA-2048bit Public",  260), // key n : 256byte + key e 4byyte
            new KeyInfo( KeyType.RSA2048bit_Private,   "RSA-2048bit Private", 512), // key n : 256byte + key d 256byte
            new KeyInfo( KeyType.DES,                  "DES",                 8),
            new KeyInfo( KeyType.DES2Key,              "2Key-TDES",           16),
            new KeyInfo( KeyType.TripleDES,            "Triple-DES",          24),
            new KeyInfo( KeyType.UpdateKeyRing,        "Update Key Ring",     32),
        };

        /// <summary>
        /// KeyInfo list (for TSIP-Lite)
        /// </summary>
        static readonly List<KeyInfo> KeyInfoList_Lite = new List<KeyInfo>()
        {
            new KeyInfo( KeyType.AES128bit,            "AES-128bit",          16),
            new KeyInfo( KeyType.AES256bit,            "AES-256bit",          32),
            new KeyInfo( KeyType.UpdateKeyRing,        "Update Key Ring",     32),
        };

        /// <summary>
        /// Address map information struct
        /// </summary>
        public class AddressMap
        {
            /// <summary>
            /// user program hardware id
            /// </summary>
            public uint hardwareId;
            /// <summary>
            /// user program top address
            /// </summary>
            public uint userProgramTopAddress;
            /// <summary>
            /// user program bottom address
            /// </summary>
            public uint userProgramBottomAddress;
            /// <summary>
            /// user program mirror top address
            /// </summary>
            public uint userProgramMirrorTopAddress;
            /// <summary>
            /// user program mirror bottom address
            /// </summary>
            public uint userProgramMirrorBottomAddress;
            /// <summary>
            /// code flash top address
            /// </summary>
            public uint codeFlashTopAddress;
            /// <summary>
            /// code flash bottom address
            /// </summary>
            public uint codeFlashBottomAddress;
            /// <summary>
            /// user const data top address
            /// </summary>
            public uint userProgramConstDataTopAddress;
            /// <summary>
            /// user const data bottom address
            /// </summary>
            public uint userProgramConstDataBottomAddress;
            /// <summary>
            /// data flash top address
            /// </summary>
            public uint dataFlashTopAddress;
            /// <summary>
            /// data flash bottom address
            /// </summary>
            public uint dataFlashBottomAddress;

            /// <summary>
            /// constructor
            /// </summary>
            /// <param name="hardware_id"></param>
            /// <param name="user_program_top_address"></param>
            /// <param name="user_program_bottom_address"></param>
            /// <param name="user_program_mirror_top_address"></param>
            /// <param name="user_program_mirror_bottom_address"></param>
            /// <param name="code_flash_top_address"></param>
            /// <param name="code_flash_bottom_address"></param>
            /// <param name="user_program_const_data_top_address"></param>
            /// <param name="user_program_const_data_bottom_address"></param>
            /// <param name="data_flash_top_address"></param>
            /// <param name="data_flash_bottom_address"></param>
            public AddressMap(
                uint hardware_id,
                uint user_program_top_address,
                uint user_program_bottom_address,
                uint user_program_mirror_top_address,
                uint user_program_mirror_bottom_address,
                uint code_flash_top_address,
                uint code_flash_bottom_address,
                uint user_program_const_data_top_address,
                uint user_program_const_data_bottom_address,
                uint data_flash_top_address,
                uint data_flash_bottom_address)
            {
                hardwareId = hardware_id;
                userProgramTopAddress = user_program_top_address;
                userProgramBottomAddress = user_program_bottom_address;
                userProgramMirrorTopAddress = user_program_mirror_top_address;
                userProgramMirrorBottomAddress = user_program_mirror_bottom_address;
                codeFlashTopAddress = code_flash_top_address;
                codeFlashBottomAddress = code_flash_bottom_address;
                userProgramConstDataTopAddress = user_program_const_data_top_address;
                userProgramConstDataBottomAddress = user_program_const_data_bottom_address;
                dataFlashTopAddress = data_flash_top_address;
                dataFlashBottomAddress = data_flash_bottom_address;
            }
        }

        const string MCUROM_RX65N_2M_SB_64KB = "RX65N(ROM 2MB)/Secure Bootloader=64KB";
        const string MCUROM_RX65N_2M_SB_256KB = "RX65N(ROM 2MB)/Secure Bootloader=256KB";
        const string MCUROM_RX231_512K_SB_32KB = "RX231(ROM 512KB)/Secure Bootloader=64KB";
        const string MCUROM_RX231_384K_SB_32KB = "RX231(ROM 384KB)/Secure Bootloader=32KB";
        const string MCUROM_RX66T_512K_SB_64KB = "RX66T(ROM 512KB)/Secure Bootloader=64KB";
        const string MCUROM_RX66T_256K_SB_64KB = "RX66T(ROM 256KB)/Secure Bootloader=64KB";
        const string MCUROM_RX72T_1M_SB_64KB = "RX72T(ROM 1MB)/Secure Bootloader=64KB";
        const string MCUROM_RX72T_512K_SB_64KB = "RX72T(ROM 512KB)/Secure Bootloader=64KB";

        const string FIRMWARE_VERIFICATION_TYPE_HASH_SHA256 = "hash-sha256";
        const string FIRMWARE_VERIFICATION_TYPE_SIG_SHA256_ECDSA = "sig-sha256-ecdsa";
        const string FIRMWARE_VERIFICATION_TYPE_MAC_AES128_CMAC_WITH_TSIP = "mac-aes128-cmac-with-tsip";
        const string FIRMWARE_VERIFICATION_TYPE_SIG_SHA256_ECDSA_WITH_TSIP = "sig-sha256-ecdsa-with-tsip";

        /// <summary>
        /// For Firm Update Tab - MCU name / Memory map
        /// </summary>
        public static readonly Dictionary<string, AddressMap> McuSpecs = new Dictionary<string, AddressMap>()
        {
            /* name (SB means Secure Bootloader) */
            { MCUROM_RX65N_2M_SB_64KB,                  new AddressMap(0x00000001, 0xfff00300, 0xfffeffff, 0xffe00300, 0xffffffff, 0xfff00000, 0xffffffff, 0x00100000, 0x00107fff, 0x00100000, 0x00107fff) },
            { MCUROM_RX65N_2M_SB_256KB,                 new AddressMap(0x00000002, 0xfff00300, 0xfffbffff, 0xffe00300, 0xfffbffff, 0xfff00000, 0xffffffff, 0x00100000, 0x00107fff, 0x00100000, 0x00107fff) },
            { MCUROM_RX231_512K_SB_32KB,                new AddressMap(0x00000003, 0,0,0,0,0,0,0,0,0,0/* under construction */) },
            { MCUROM_RX231_384K_SB_32KB,                new AddressMap(0x00000004, 0,0,0,0,0,0,0,0,0,0/* under construction */) },
            { MCUROM_RX66T_512K_SB_64KB,                new AddressMap(0x00000005, 0,0,0,0,0,0,0,0,0,0/* under construction */) },
            { MCUROM_RX66T_256K_SB_64KB,                new AddressMap(0x00000006, 0,0,0,0,0,0,0,0,0,0/* under construction */) },
            { MCUROM_RX72T_1M_SB_64KB,                  new AddressMap(0x00000007, 0,0,0,0,0,0,0,0,0,0/* under construction */) },
            { MCUROM_RX72T_512K_SB_64KB,                new AddressMap(0x00000008, 0,0,0,0,0,0,0,0,0,0/* under construction */) },
        };

        public static readonly Dictionary<string, uint> FirmVerificationType = new Dictionary<string, uint>()
        {
            { FIRMWARE_VERIFICATION_TYPE_HASH_SHA256, 1 },
            { FIRMWARE_VERIFICATION_TYPE_SIG_SHA256_ECDSA, 2 },
            { FIRMWARE_VERIFICATION_TYPE_MAC_AES128_CMAC_WITH_TSIP, 3 },
            { FIRMWARE_VERIFICATION_TYPE_SIG_SHA256_ECDSA_WITH_TSIP, 4 },
        };

        public class rsu_header
        {
            /*
            ----------------------------------------------------------------------------------------------------
            output *.rsu
            reference: https://docs.aws.amazon.com/ja_jp/freertos/latest/userguide/microchip-bootloader.html
            ----------------------------------------------------------------------------------------------------
            offset              component           contents name               length(byte)    OTA Image(Signed area)
            0x00000000          Header              Magic Code                  7
            0x00000007                              Image Flags                 1
            0x00000008          Signature           Firmware Verification Type  32
            0x00000028                              Signature size              4
            0x0000002c                              Signature                   256
            0x0000012c          Option              Dataflash Flag              4
            0x00000130                              Dataflash Start Address     4
            0x00000134                              Dataflash End Address       4
            0x00000138                              Resereved(0x00)             200
            0x00000200          Descriptor          Sequence Number             4               ---
            0x00000204                              Start Address               4                |
            0x00000208                              End Address                 4                |
            0x0000020c                              Execution Address           4                |
            0x00000210                              Hardware ID                 4                |
            0x00000214                              Resereved(0x00)             236              |
            0x00000300          Application Binary                              N               --- <- provided as mot file
            0x00000300 + N      Dataflash Binary                                M                   <- provided as mot file
            ----------------------------------------------------------------------------------------------------
            Magic Code              : Renesas
            Image Flags             : 0xff アプリケーションイメージは新しく、決して実行されません。
                                    　0xfe アプリケーションイメージにテスト実行のためのマークが付けられます。
                                      0xfc アプリケーションイメージが有効とマークされ、コミットされます。
                                      0xf8 アプリケーションイメージは無効とマークされています。
            Firmware Verification Type
                                    : ファームウェア検証方式を指定するための識別子です。
                                      例: sig-sha256-ecdsa
            Signature/MAC/Hash size : ファームウェア検証に用いる署名値やMAC値やハッシュ値などのデータサイズです。
            Signature/MAC/Hash      : ファームウェア検証に用いる署名値やMAC値やハッシュ値です。
            Sequence Number         : シーケンス番号は、新しい OTA イメージを構築する前に増加させる必要があります。
                                    　Renesas Secure Flash Programmerにてユーザが指定可能です。
                                      ブートローダーは、この番号を使用してブートするイメージを決定します。
                                      有効な値の範囲は 1～ 4294967295‬ です。 
            Start Address           : デバイス上のOTA Imageの開始アドレスです。
                                      Renesas Secure Flash Programmerが自動的に設定するため、ユーザ指定は不要です。
            End Address             : イメージトレーラーを除く、デバイス上のOTA Imageの終了アドレスです。
                                      Renesas Secure Flash Programmerが自動的に設定するため、ユーザ指定は不要です。
            Hardware ID             : OTA Imageが正しいプラットフォーム用に構築されているかどうかを検証するために
                                      ブートローダーによって使用される一意のハードウェア ID です。
                                      例: 0x00000001    MCUROM_RX65N_2M_SB_64KB
            */
            public byte[] magic_code = new byte[7];
            public byte image_flag;
            public byte[] signature_type = new byte[32];
            public UInt32 signature_size;
            public byte[] signature = new byte[256];
            public UInt32 dataflash_flag;
            public UInt32 dataflash_start_address;
            public UInt32 dataflash_end_address;
            public byte[] reserved1 = new byte[200];
            public UInt32 sequence_number;
            public UInt32 start_address;
            public UInt32 end_address;
            public UInt32 execution_address;
            public UInt32 hardware_id;
            public byte[] reserved2 = new byte[236];
        }

        const string STR_RAMDOM_DATA_GENERATE = "(Random)";
        const int SESSION_KEY_BYTE_SIZE = 32;
        const int IV_MAC_BYTE_SIZE = 16;
        const int USER_PROGRAM_KEY_BYTE_SIZE = 16;
        const int HEADER_LINE_INSERT_INDEX = 72;
        const int SOURCE_LINE_INSERT_INDEX = 77;
        const int CODE_FLASH_SIGNATURE_AREA_OFFSET = 0x200;
        const int IMAGE_FLAG_BLANK = 0xff;
        const int IMAGE_FLAG_TESTING = 0xfe;
        const int IMAGE_FLAG_INSTALLING = 0xfc;
        const int IMAGE_FLAG_VALID = 0xf8;
        const int IMAGE_FLAG_INVALID = 0xf0;
        const int CommandLineArgumentFirst = 0;
        const int ARGUMENT_INTERFACE_TYPE = 0;                              // GUI, CUI
        const int ARGUMENT_FIRMWARE_TYPE = 1;                               // Initial, Update
        const int ARGUMENT_INITIAL_MCU = 2;                                 // "RX65N(ROM 2MB)/Secure Bootloader=256KB"
        const int ARGUMENT_INITIAL_INPUT_FIRMWARE_VERIFICATION_TYPE = 3;    // "sig-sha256-ecdsa"
        const int ARGUMENT_INITIAL_INPUT_FIRMWARE_SEQUENCE_NUMBER = 4;      // 1
        const int ARGUMENT_INITIAL_INPUT_BOOTLOADER_FILE_PATH = 5;          // "C:\\amazon-rx\\amazon-freertos\\demos\\renesas\\rx65n-rsk\\e2studio\\ccrx\\boot_loader\\type2\\HardwareDebug\\boot_loader.mot"
        const int ARGUMENT_INITIAL_INPUT_FIRMWARE_FILE_PATH = 6;            // "C:\\amazon-rx\\amazon-freertos\\tests\\renesas\\rx65n-rsk\\e2studio\\HardwareDebug\\aws_tests.mot"
        const int ARGUMENT_INITIAL_INPUT_PRIVATE_KEY_PATH = 7;              // "C:\\Users\\a5090440\\Desktop\\newfolder\\openssl\\device_tester_basico0303\\server.privatekey"
        const int ARGUMENT_INITIAL_OUTPUT_FILE_PATH = 8;                    // "C:\\amazon-rx\\amazon-freertos\\tests\\renesas\\rx65n-rsk\\e2studio\\HardwareDebug"
        const int ARGUMENT_UPDATE_MCU = 2;                                  // "RX65N(ROM 2MB)/Secure Bootloader=256KB"
        const int ARGUMENT_UPDATE_INPUT_FIRMWARE_VERIFICATION_TYPE = 3;    // "sig-sha256-ecdsa"
        const int ARGUMENT_UPDATE_INPUT_FIRMWARE_SEQUENCE_NUMBER = 4;      // 1
        const int ARGUMENT_UPDATE_INPUT_FIRMWARE_FILE_PATH = 5;            // "C:\\amazon-rx\\amazon-freertos\\tests\\renesas\\rx65n-rsk\\e2studio\\HardwareDebug\\aws_tests.mot"
        const int ARGUMENT_UPDATE_OUTPUT_FILE_PATH = 6;                    // "C:\\amazon-rx\\amazon-freertos\\tests\\renesas\\rx65n-rsk\\e2studio\\HardwareDebug"
        const string FIRMWARE_TYPE_INITIAL = "Initial";
        const string FIRMWARE_TYPE_UPDATE = "Update";
        const string INITIAL_FIRM_MOT_S0_FORMAT = "0F00006177735F746573742E6D6F74";
        private int log_count = 0;

        OpenFileDialog openFileDialog = new OpenFileDialog();
        SaveFileDialog saveFileDialog = new SaveFileDialog();

        // Create AesCryptoServiceProvider Object
        AesCryptoServiceProvider aesCryptoProvider = new AesCryptoServiceProvider();

        /// <summary>
        /// Constructor
        /// </summary>
        public FormMain(string[] args)
        {
            InitializeComponent();


            //Set aes propery
            aesCryptoProvider.BlockSize = 128;
            aesCryptoProvider.Mode = CipherMode.ECB;
            aesCryptoProvider.Padding = PaddingMode.None;
            aesCryptoProvider.KeySize = 128;
            if ((args.Length != 0) && (args[ARGUMENT_INTERFACE_TYPE] == "CUI"))
            {
                string mcuName;
                checkBox1_InitialOutputBinaryFormat.Checked = false;
                if (args[ARGUMENT_FIRMWARE_TYPE] == FIRMWARE_TYPE_INITIAL)
                {
                    Console.WriteLine("Reneas Secure Flash Programmer CUI");
                    Console.WriteLine("Start generating initial firmware in Renesas Secure Update File");
                    /* 引数設定  */
                    mcuName = args[ARGUMENT_INITIAL_MCU];
                    comboBoxInitialFirmwareVerificationType.Text = args[ARGUMENT_INITIAL_INPUT_FIRMWARE_VERIFICATION_TYPE];
                    textBoxInitialFirmwareSequenceNumber.Text = args[ARGUMENT_INITIAL_INPUT_FIRMWARE_SEQUENCE_NUMBER];
                    textBoxInitialBootLoaderUserProgramFilePath.Text = args[ARGUMENT_INITIAL_INPUT_BOOTLOADER_FILE_PATH];
                    textBoxInitialUserProgramFilePath.Text = args[ARGUMENT_INITIAL_INPUT_FIRMWARE_FILE_PATH];
                    textBoxInitialUserPrivateKeyPath.Text = args[ARGUMENT_INITIAL_INPUT_PRIVATE_KEY_PATH];
                    saveFileDialog.FileName = args[ARGUMENT_INITIAL_OUTPUT_FILE_PATH];
                    comboBox_Initial_Mcu_firmupdate.Text = mcuName;
                    GenerateInitialUserprog(mcuName);
                }
                else if (args[ARGUMENT_FIRMWARE_TYPE] == FIRMWARE_TYPE_UPDATE)
                {/* FIRMWARE_TYPE_UPDATE */
                    Console.WriteLine("Reneas Secure Flash Programmer CUI");
                    Console.WriteLine("Start generating update firmware in Motorola S Format File");

                    mcuName = args[ARGUMENT_INITIAL_MCU];
                    comboBoxFirmwareVerificationType.Text = args[ARGUMENT_UPDATE_INPUT_FIRMWARE_VERIFICATION_TYPE];
                    textBoxFirmwareSequenceNumber.Text = args[ARGUMENT_UPDATE_INPUT_FIRMWARE_SEQUENCE_NUMBER];
                    textBoxUserProgramFilePath.Text = args[ARGUMENT_UPDATE_INPUT_FIRMWARE_FILE_PATH];
                    saveFileDialog.FileName = args[ARGUMENT_UPDATE_OUTPUT_FILE_PATH];
                    comboBoxMcu_firmupdate.Text = mcuName;
                    GenerateUserprog(mcuName);
                }
                else
                {
                    Console.WriteLine("Reneas Secure Flash Programmer CUI\r\n");
                    Console.WriteLine("CUI Initial [MCU] [Verification Type] [Firmware Sequence No] [Boot Loader File Path]  [User Program File Path] [Private Key Path(GER)] [Output File Path]\r\n");
                    Console.WriteLine("CUI Update [MCU] [Verification Type] [Firmware Sequence No] [User Program File Path] [Output File Path]\r\n");
                }
            }
        }
        /// <summary>
        /// Calculate motorola checksum
        /// </summary>
        /// <param name="dataToCalculate"></param>
        private string CalculateMotorolaChecksum(string dataToCalculate)
        {
            var byteToCalculate = new List<byte>();
            for (int i = 0; i < dataToCalculate.Length / 2; i++)
            {
                byteToCalculate.Add(Convert.ToByte(dataToCalculate.Substring(i * 2, 2), 16));
            }

            int checksum = 0;
            foreach (byte chData in byteToCalculate)
            {
                checksum += chData;
            }
            checksum = ~checksum & 0xFF;
            return checksum.ToString("X");

        }

        /// <summary>
        /// Form loading
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void FormMain_Load(object sender, EventArgs e)
        {
            // initialize Session Key Tab

            // [Session key Value] textbox 
            textBoxSessionKey.Text = STR_RAMDOM_DATA_GENERATE;

            // initialize Key Wrap Tab

            // [Select Mcu] combobox 
            foreach (var mcu in Enum.GetValues(typeof(Mcu)))
            {
                comboBoxMcu_keywrap.Items.Add(mcu);
            }

            // [Endian] combobox
            DataTable endianTable = new DataTable();
            endianTable.Columns.Add("DISPSTRING", typeof(string));
            endianTable.Columns.Add("VALUE", typeof(Endian));
            foreach (var endian in Enum.GetValues(typeof(Endian)))
            {
                var newRow = endianTable.NewRow();
                newRow["DISPSTRING"] = $"{endian} Endian";
                newRow["VALUE"] = endian;
                endianTable.Rows.Add(newRow);
            }

            comboBoxEndian.DataSource = endianTable;
            comboBoxEndian.DisplayMember = "DISPSTRING";
            comboBoxEndian.ValueMember = "VALUE";

            // [Key Type] combbox
            setKeyTypeList(Mcu.RX231);

            // [IV] textbox 
            textBoxIV.Text = STR_RAMDOM_DATA_GENERATE;

            // initialize Firm Update tab

            // [Select Mcu] combobox 
            foreach (var mcu in McuSpecs)
            {
                comboBoxMcu_firmupdate.Items.Add(mcu.Key);
            }
            // [Select Mcu] combobox 
            foreach (var mcu in FirmVerificationType)
            {
                comboBoxFirmwareVerificationType.Items.Add(mcu.Key);
            }

            // initialize Initial  tab

            // [Select Mcu] combobox 
            foreach (var mcu in McuSpecs)
            {
                comboBox_Initial_Mcu_firmupdate.Items.Add(mcu.Key);
            }
            // [Select Mcu] combobox 
            foreach (var mcu in FirmVerificationType)
            {
                comboBoxInitialFirmwareVerificationType.Items.Add(mcu.Key);
            }
        }

        /// <summary>
        /// create [Key Type] list
        /// </summary>
        /// <param name="mcu"></param>
        private void setKeyTypeList(Mcu mcu)
        {
            List<KeyInfo> keyInfoList = null;

            if (getTSIPFunctionLevel(mcu) == TSIPLevel.Full)
            {
                keyInfoList = KeyInfoList_Full;
            }
            else
            {
                keyInfoList = KeyInfoList_Lite;
            }

            DataTable keyinfoTable = new DataTable();
            keyinfoTable.Columns.Add("DISPSTRING", typeof(string));
            keyinfoTable.Columns.Add("VALUE", typeof(KeyInfo));
            foreach (var keyinfo in keyInfoList)
            {
                var newRow = keyinfoTable.NewRow();
                newRow["DISPSTRING"] = keyinfo.Name;
                newRow["VALUE"] = keyinfo;
                keyinfoTable.Rows.Add(newRow);
            }

            comboBoxKeyType.DataSource = keyinfoTable;
            comboBoxKeyType.DisplayMember = "DISPSTRING";
            comboBoxKeyType.ValueMember = "VALUE";

            // claer unsupported key data
            for (int i = listViewKeys.Items.Count - 1; i >= 0; --i)
            {
                if (!keyInfoList.Exists(info => info.Name == listViewKeys.Items[i].Text))
                {
                    listViewKeys.Items.RemoveAt(i);
                }
            }
        }

        #region [Session Key] Tab

        /// <summary>
        /// Open DLM server link
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void linkLabelDLMServer_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            linkLabelDLMServer.LinkVisited = true;
            Process.Start(linkLabelDLMServer.Text);
        }

        /// <summary>
        /// [Session Key] Tab - [Generate Session Key] button
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void buttonGenerateSessionKey_Click(object sender, EventArgs e)
        {
            saveFileDialog.Filter = "Session Key File|*.key";
            saveFileDialog.Title = "Specify the Output File Name";
            saveFileDialog.FileName = "";

            if (saveFileDialog.ShowDialog() != DialogResult.OK || saveFileDialog.FileName == "")
            {
                print_log("please specify the output file name.");
                return;
            }

            // Create session key data
            byte[] session_key = new byte[SESSION_KEY_BYTE_SIZE];

            string strSessionKey = textBoxSessionKey.Text;
            if (strSessionKey == STR_RAMDOM_DATA_GENERATE)  // (Random)
            {
                RNGCryptoServiceProvider rng = new RNGCryptoServiceProvider();
                rng.GetBytes(session_key);
            }
            else
            {
                if (strSessionKey.Length != SESSION_KEY_BYTE_SIZE * 2) // must be 64 chars
                {
                    print_log("please specify the correct key size.");
                    return;
                }

                try
                {
                    session_key = convertStrDataToKeyData(strSessionKey, SESSION_KEY_BYTE_SIZE);
                }
                catch (Exception)
                {
                    print_log("exception has occurred.");
                    return;
                }
            }

            // Write binary file
            try
            {
                File.WriteAllBytes(saveFileDialog.FileName, session_key);
                print_log("generate succeeded.");
            }
            catch (Exception)
            {
                print_log("exception has occurred.");
            }
        }

        #endregion [Session Key] Tab

        #region [Key Wrap] Tab

        /// <summary>
        /// [Key Wrap] Tab - [Select MCU] list changed handler
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void comboBoxMcu_keywrap_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboBoxMcu_keywrap.SelectedIndex > -1)
            {
                Mcu mcu = (Mcu)Enum.Parse(typeof(Mcu), comboBoxMcu_keywrap.Text);
                setKeyTypeList(mcu);
            }
        }

        /// <summary>
        /// [Key Wrap] Tab - [Register] button
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void buttonRegister_Click(object sender, EventArgs e)
        {
            // check Key Type selection
            if (comboBoxKeyType.SelectedIndex < 0)
            {
                print_log("please select Key Type in Key Setting.");
                return;
            }

            // check Key Data length
            KeyInfo selectedKeyInfo = (KeyInfo)comboBoxKeyType.SelectedValue;
            Debug.Assert(selectedKeyInfo != null);

            if (textBoxKeyData.Text.Length != selectedKeyInfo.DataLength * 2)
            {
                print_log("please specify the correct key size.");
                return;
            }

            // register Key Data
            ListViewItem keyData = new ListViewItem();
            keyData.Tag = selectedKeyInfo;
            keyData.Text = selectedKeyInfo.Name;
            keyData.SubItems.Add(textBoxKeyData.Text);
            listViewKeys.Items.Add(keyData);

            listViewKeys.ListViewItemSorter = new ListViewItemComparer();
            listViewKeys.Sort();
        }

        /// <summary>
        /// ListViewItem Sort class
        /// </summary>
        private class ListViewItemComparer : IComparer
        {
            /// <summary>
            /// ListViewItemComparer constructor
            /// </summary>
            public ListViewItemComparer() { }

            /// <summary>
            /// compare by KeyType enum
            /// </summary>
            /// <param name="x"></param>
            /// <param name="y"></param>
            /// <returns></returns>
            public int Compare(object x, object y)
            {
                // get KeyInfo from ListViewItem
                KeyInfo infoX = (KeyInfo)((ListViewItem)x).Tag;
                KeyInfo infoY = (KeyInfo)((ListViewItem)y).Tag;

                // x < y  - minus value
                // x == y - zero
                // x > y  - plus value
                return infoX.Type - infoY.Type;
            }
        }

        /// <summary>
        /// [Key Wrap] Tab - [Delete] button
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void buttonDelete_Click(object sender, EventArgs e)
        {
            // delete selected item
            if (listViewKeys.SelectedItems.Count > 0)
            {
                var selectedIndex = listViewKeys.SelectedItems[0].Index;
                Debug.Assert(selectedIndex > -1);
                listViewKeys.Items.RemoveAt(selectedIndex);
            }
        }

        /// <summary>
        /// [Key Wrap] Tab - [Browse...] button of [Session Key File Path]
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void buttonBrowseSessionKey_Click(object sender, EventArgs e)
        {
            openFileDialog.Filter = "Session Key File|*.key";
            openFileDialog.Title = "Specify the Session Key File Name";
            openFileDialog.FileName = "";

            if (openFileDialog.ShowDialog() != DialogResult.OK || openFileDialog.FileName == "")
            {
                print_log("please specify the session key file name.");
                return;
            }

            textBoxSessionKeyPath.Text = openFileDialog.FileName;
        }

        /// <summary>
        /// [Key Wrap] Tab - [Browse...] button of [Encrypted Session Key File Path]
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void buttonBrowseEncryptedSessionKey_Click(object sender, EventArgs e)
        {
            openFileDialog.Filter = "Encrypted Session Key File|*.key";
            openFileDialog.Title = "Specify the Encrypted Session Key File Name";
            openFileDialog.FileName = "";

            if (openFileDialog.ShowDialog() != DialogResult.OK || openFileDialog.FileName == "")
            {
                print_log("please specify the encrypted session key file name.");
                return;
            }

            textBoxEncryptedSessionKeyPath.Text = openFileDialog.FileName;

        }

        /// <summary>
        /// [Key Wrap] Tab - [Generate Key File...] button
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void buttonGenerateKeyFile_Click(object sender, EventArgs e)
        {
            // check MCU selection
            if (comboBoxMcu_keywrap.SelectedIndex < 0)
            {
                print_log("please select MCU type.");
                return;
            }
            // check Key List
            if (listViewKeys.Items.Count == 0)
            {
                print_log("please specify the key data.");
                return;
            }
            // check Session Key File Path
            if (!File.Exists(textBoxSessionKeyPath.Text))
            {
                print_log("please specify the session key file name.");
                return;
            }
            // check Encrypted Session Key File Path
            if (!File.Exists(textBoxEncryptedSessionKeyPath.Text))
            {
                print_log("please specify the encrypted session key file name.");
                return;
            }
            // check IV
            if (textBoxIV.Text != STR_RAMDOM_DATA_GENERATE)  // not random
            {
                if (textBoxIV.Text.Length != IV_MAC_BYTE_SIZE * 2) // must be 32 chars
                {
                    print_log("please specify the correct iv size.");
                    return;
                }
            }
            // Displays a SaveFileDialog_header so the user can save
            saveFileDialog.Filter = "Key data header|*.h";
            saveFileDialog.Title = "Save a key data header File";
            saveFileDialog.FileName = "key_data.h";

            if (saveFileDialog.ShowDialog() != DialogResult.OK || saveFileDialog.FileName == "")
            {
                print_log("please specify the output header file name.");
                return;
            }
            string keyDataHeaderPath = saveFileDialog.FileName;
            // Displays a SaveFileDialog_source so the user can save
            saveFileDialog.Filter = "Key data source|*.c";
            saveFileDialog.Title = "Save a key data File";
            saveFileDialog.FileName = "key_data.c";

            if (saveFileDialog.ShowDialog() != DialogResult.OK || saveFileDialog.FileName == "")
            {
                print_log("please specify the output source file name.");
                return;
            }
            string keyDataSourcePath = saveFileDialog.FileName;

            try
            {
                // get MCU type and Endian
                Mcu mcu = (Mcu)Enum.Parse(typeof(Mcu), comboBoxMcu_keywrap.Text);
                Endian endian = (Endian)comboBoxEndian.SelectedValue;

                // get user key datas form Key List and convert to byte data
                List<Tuple<KeyInfo, byte[]>> userKeyDataList = new List<Tuple<KeyInfo, byte[]>>();
                foreach (ListViewItem item in listViewKeys.Items)
                {
                    KeyInfo keyInfo = (KeyInfo)item.Tag;
                    string strKeyData = item.SubItems[1].Text;

                    byte[] userKeyData = createUserKeyData(keyInfo, strKeyData);

                    userKeyDataList.Add(new Tuple<KeyInfo, byte[]>(keyInfo, userKeyData));
                }

                // get Session Key File data
                byte[] sessionKey = File.ReadAllBytes(textBoxSessionKeyPath.Text);

                // get Encrypted Session Key File data
                byte[] encryptedSessionKey = File.ReadAllBytes(textBoxEncryptedSessionKeyPath.Text);

                // get IV
                byte[] iv = new byte[IV_MAC_BYTE_SIZE];
                if (textBoxIV.Text == STR_RAMDOM_DATA_GENERATE)  // (Random)
                {
                    RNGCryptoServiceProvider rng = new RNGCryptoServiceProvider();
                    rng.GetBytes(iv);
                }
                else
                {
                    iv = convertStrDataToKeyData(textBoxIV.Text, IV_MAC_BYTE_SIZE);
                }

                // create key_data.h and key_data.c
                outputKeyDataFiles(mcu, endian, keyDataHeaderPath, keyDataSourcePath,
                    userKeyDataList, sessionKey, encryptedSessionKey, iv);

                print_log("generate succeeded.");
            }
            catch (Exception)
            {
                print_log("exception has occurred.");
            }
        }

        /// <summary>
        /// create user key data from string key data
        /// </summary>
        /// <param name="keyInfo"></param>
        /// <param name="strKeyData"></param>
        /// <returns></returns>
        private byte[] createUserKeyData(KeyInfo keyInfo, string strKeyData)
        {
            int keyLength;
            switch (keyInfo.Type)
            {
                case KeyType.RSA1024bit_Public:
                case KeyType.RSA2048bit_Public:
                    keyLength = keyInfo.DataLength + 12; // "12" is 0 padding length
                    break;
                case KeyType.DES:
                    strKeyData = string.Concat(Enumerable.Repeat(strKeyData, 3)); // DES user key * 3
                    keyLength = 32;
                    break;
                case KeyType.DES2Key:
                    strKeyData += strKeyData.Substring(0, 16);
                    keyLength = 32;
                    break;
                case KeyType.TripleDES:
                    keyLength = 32;
                    break;
                default:
                    keyLength = keyInfo.DataLength;
                    break;
            }

            return convertStrDataToKeyData(strKeyData, keyLength);
        }

        /// <summary>
        /// output key_data.h and key_data.c
        /// </summary>
        /// <param name="mcu"></param>
        /// <param name="endian"></param>
        /// <param name="keyDataHeaderPath"></param>
        /// <param name="keyDataSourcePath"></param>
        /// <param name="keyDataList"></param>
        /// <param name="session_key"></param>
        /// <param name="encrypted_session_key"></param>
        /// <param name="iv"></param>
        private void outputKeyDataFiles(Mcu mcu, Endian endian, string keyDataHeaderPath, string keyDataSourcePath,
            List<Tuple<KeyInfo, byte[]>> keyDataList, byte[] sessionKey, byte[] encryptedSessionKey, byte[] iv)
        {
            List<string> keyDataDeclarations = new List<string>();  // a list of key data declarations to insert into key_data.h
            List<string> keyIndexDeclarations = new List<string>(); // a list of key index declarations to insert into key_data.h
            List<string> keyDataDefinitions = new List<string>();   // a list of key data definitions to insert into key_data.c
            List<string> keyIndexDefinitions = new List<string>();  // a list of key index definitions to insert into key_data.c

            byte[] sha1Data = new byte[40]; // 40 is size of st_firmware_update_control_block_t
            Array.Clear(sha1Data, 0, sha1Data.Length);
            sha1Data = sha1Data.Concat(encryptedSessionKey.Skip(4).ToArray()).ToArray();
            sha1Data = sha1Data.Concat(iv).ToArray();

            byte[] keyIndexData = new byte[0];

            // divide session key to CBC key and CBC MAC key
            byte[] keyCBC = new byte[IV_MAC_BYTE_SIZE];
            byte[] keyCBCMAC = new byte[IV_MAC_BYTE_SIZE];
            Array.Copy(sessionKey, 0, keyCBC, 0, IV_MAC_BYTE_SIZE);
            Array.Copy(sessionKey, IV_MAC_BYTE_SIZE, keyCBCMAC, 0, IV_MAC_BYTE_SIZE);

            int sameKeyCount = 1;
            KeyType prevousKeyType = KeyType.AES128bit;
            foreach (var keyDataTuple in keyDataList)
            {
                KeyInfo keyInfo = keyDataTuple.Item1;
                byte[] keyData = keyDataTuple.Item2;

                if (keyInfo.Type == KeyType.DES || keyInfo.Type == KeyType.DES2Key || keyInfo.Type == KeyType.TripleDES)
                {
                    if (prevousKeyType != KeyType.DES && prevousKeyType != KeyType.DES2Key && prevousKeyType != KeyType.TripleDES)
                    {
                        prevousKeyType = keyInfo.Type;
                        sameKeyCount = 1;
                    }
                }
                else if (keyInfo.Type != prevousKeyType)
                {
                    prevousKeyType = keyInfo.Type;
                    sameKeyCount = 1;
                }

                // create key data/key index declaratoin line
                var keyDataDeclarationItems = getKeyDataDeclarationText(keyInfo);
                var keyIndexDeclarationItems = getKeyIndexDeclarationData(keyInfo);
                string strKeyDataType = keyDataDeclarationItems.Item1;
                string strKeyDataName = keyDataDeclarationItems.Item2;
                string strKeyIndexType = keyIndexDeclarationItems.Item1;
                string strKeyIndexName = keyIndexDeclarationItems.Item2;
                if (sameKeyCount > 1)
                {
                    strKeyDataName = strKeyDataName.Replace("_key[", $"_key{sameKeyCount}[");
                    strKeyIndexName = strKeyIndexName.Replace("_index;", $"_index{sameKeyCount};");
                }
                keyDataDeclarations.Add($"        {strKeyDataType}{new string(' ', 41 - 8 - strKeyDataType.Length)}{strKeyDataName}");
                keyIndexDeclarations.Add($"        {strKeyIndexType}{new string(' ', 41 - 8 - strKeyIndexType.Length)}{strKeyIndexName}");

                // create key data/key index definition line
                byte[] encryptedKeyData = encryptKeyData(keyData, keyCBC, keyCBCMAC, iv);
                keyDataDefinitions.Add($"        /* {strKeyDataType} {strKeyDataName} */");
                keyDataDefinitions.Add(byteArrayToSourceText(encryptedKeyData, 8));

                keyIndexDefinitions.Add($"        /* {strKeyIndexType} {strKeyIndexName} */");
                keyIndexDefinitions.Add("        {");
                keyIndexDefinitions.Add("            0");
                keyIndexDefinitions.Add("        },");

                // add encrypted key data/key index
                byte[] keyIndexDataTmp = Enumerable.Repeat<byte>(0x00, keyIndexDeclarationItems.Item3).ToArray();
                keyIndexData = keyIndexData.Concat(keyIndexDataTmp).ToArray();
                sha1Data = sha1Data.Concat(encryptedKeyData).ToArray();

                ++sameKeyCount;
            }
            // add key index data
            sha1Data = sha1Data.Concat(keyIndexData).ToArray();
            // calculate SHA-1 hash
            SHA1CryptoServiceProvider sha1Provider = new SHA1CryptoServiceProvider();
            byte[] sha1Hash = sha1Provider.ComputeHash(sha1Data);

            // replace filename/encrypted sessoin key/iv/SHA1 hash in template text
            string strEncryptedSessionKey = byteArrayToSourceText(encryptedSessionKey.Skip(4).ToArray(), 8); // do not ouput first 4 bytes(HRK).
            string strIV = byteArrayToSourceText(iv, 8);
            string strSHA1Hash = byteArrayToSourceText(sha1Hash, 4);

            var templateTexts = generateTemplateText(mcu, keyDataHeaderPath, keyDataSourcePath,
                strEncryptedSessionKey, strIV, strSHA1Hash);
            List<string> headerTexts = templateTexts.Item1.Split(new string[] { "\r\n" }, StringSplitOptions.None).ToList();
            List<string> sourceTexts = templateTexts.Item2.Split(new string[] { "\r\n" }, StringSplitOptions.None).ToList();

            // insert declarations
            headerTexts.InsertRange(HEADER_LINE_INSERT_INDEX, keyDataDeclarations);
            headerTexts.InsertRange(HEADER_LINE_INSERT_INDEX + keyDataDeclarations.Count, keyIndexDeclarations);
            // insert definitions
            sourceTexts.InsertRange(SOURCE_LINE_INSERT_INDEX, keyDataDefinitions);
            sourceTexts.InsertRange(SOURCE_LINE_INSERT_INDEX + keyDataDefinitions.Count, keyIndexDefinitions);

            // add s_flash data
            string sflash = getSFlashText(mcu, endian);
            sourceTexts.Add(sflash);

            // create output source file data
            File.WriteAllLines(keyDataHeaderPath, headerTexts.ToArray());
            File.WriteAllLines(keyDataSourcePath, sourceTexts.ToArray());
        }

        /// <summary>
        /// get s_flash defination text
        /// </summary>
        /// <param name="mcu"></param>
        /// <param name="endian"></param>
        /// <returns></returns>
        private string getSFlashText(Mcu mcu, Endian endian)
        {
            switch (mcu)
            {
                case Mcu.RX231:
                    return endian == Endian.Little ? Resources.s_flash_rx231_little : Resources.s_flash_rx231_big;
                case Mcu.RX65N:
                    return endian == Endian.Little ? Resources.s_flash_rx65n_little : Resources.s_flash_rx65n_big;
                case Mcu.RX66T:
                case Mcu.RX72T:
                    return endian == Endian.Little ? Resources.s_flash_rx66t_rx72t_little : Resources.s_flash_rx66t_rx72t_big;
                default:
                    return "";
            }
        }

        /// <summary>
        /// get user key data declaratoin string
        /// </summary>
        /// <param name="keyInfo"></param>
        /// <returns></returns>
        private Tuple<string, string> getKeyDataDeclarationText(KeyInfo keyInfo)
        {
            switch (keyInfo.Type)
            {
                case KeyType.AES128bit:
                    return new Tuple<string, string>(
                        "uint8_t",
                        "encrypted_user_aes128_key[R_TSIP_AES128_KEY_BYTE_SIZE + 16];");
                case KeyType.AES256bit:
                    return new Tuple<string, string>(
                        "uint8_t",
                        "encrypted_user_aes256_key[R_TSIP_AES256_KEY_BYTE_SIZE + 16];");
                case KeyType.RSA1024bit_Public:
                    return new Tuple<string, string>(
                        "uint8_t",
                        "encrypted_user_rsa1024_ne_key[R_TSIP_RSA1024_NE_KEY_BYTE_SIZE + 16];");
                case KeyType.RSA1024bit_Private:
                    return new Tuple<string, string>(
                        "uint8_t",
                        "encrypted_user_rsa1024_nd_key[R_TSIP_RSA1024_ND_KEY_BYTE_SIZE + 16];");
                case KeyType.RSA2048bit_Public:
                    return new Tuple<string, string>(
                        "uint8_t",
                        "encrypted_user_rsa2048_ne_key[R_TSIP_RSA2048_NE_KEY_BYTE_SIZE + 16];");
                case KeyType.RSA2048bit_Private:
                    return new Tuple<string, string>(
                        "uint8_t",
                        "encrypted_user_rsa2048_nd_key[R_TSIP_RSA2048_ND_KEY_BYTE_SIZE + 16];");
                case KeyType.DES:
                case KeyType.DES2Key:
                case KeyType.TripleDES:
                    return new Tuple<string, string>(
                        "uint8_t",
                        "encrypted_user_tdes_key[R_TSIP_TDES_KEY_BYTE_SIZE + 16];");
                case KeyType.UpdateKeyRing:
                    return new Tuple<string, string>(
                        "uint8_t",
                        "encrypted_user_update_key[R_TSIP_AES256_KEY_BYTE_SIZE + 16];");
                default:
                    return new Tuple<string, string>("", "");
            }
        }

        /// <summary>
        /// get user key index declaration string and key index size
        /// </summary>
        /// <param name="keyInfo"></param>
        /// <returns></returns>
        private Tuple<string, string, int> getKeyIndexDeclarationData(KeyInfo keyInfo)
        {
            switch (keyInfo.Type)
            {
                case KeyType.AES128bit:
                    return new Tuple<string, string, int>(
                        "tsip_aes_key_index_t",
                        "user_aes128_key_index;",
                        68);
                case KeyType.AES256bit:
                    return new Tuple<string, string, int>(
                        "tsip_aes_key_index_t",
                        "user_aes256_key_index;",
                        68);
                case KeyType.RSA1024bit_Public:
                    return new Tuple<string, string, int>(
                        "tsip_rsa1024_public_key_index_t",
                        "user_rsa1024_ne_key_index;",
                        308);
                case KeyType.RSA1024bit_Private:
                    return new Tuple<string, string, int>(
                        "tsip_rsa1024_private_key_index_t",
                        "user_rsa1024_nd_key_index;",
                        420);
                case KeyType.RSA2048bit_Public:
                    return new Tuple<string, string, int>(
                        "tsip_rsa2048_public_key_index_t",
                        "user_rsa2048_ne_key_index;",
                        564);
                case KeyType.RSA2048bit_Private:
                    return new Tuple<string, string, int>(
                        "tsip_rsa2048_private_key_index_t",
                        "user_rsa2048_nd_key_index;",
                        804);
                case KeyType.DES:
                case KeyType.DES2Key:
                case KeyType.TripleDES:
                    return new Tuple<string, string, int>(
                        "tsip_tdes_key_index_t",
                        "user_tdes_key_index;",
                        68);
                case KeyType.UpdateKeyRing:
                    return new Tuple<string, string, int>(
                        "tsip_update_key_ring_t",
                        "user_update_key_index;",
                        68);
                default:
                    return new Tuple<string, string, int>("", "", 0);
            }
        }

        /// <summary>
        /// change file name in template file text
        /// </summary>
        /// <param name="templateText"></param>
        /// <param name="filePath"></param>
        private Tuple<string, string> generateTemplateText(Mcu mcu, string headerPath, string sourcePath,
                                                           string strEncryptedSessionKey, string strIV, string strSHA1Hash)
        {
            string headerText = Resources.key_data_header_template;
            string headerNameWithoutExt = Path.GetFileNameWithoutExtension(headerPath);
            string headerNameUpper = headerNameWithoutExt.ToUpper();
            var blockTopAddress = getDataBlockStartAddress(mcu);

            headerText = headerText.Replace("{header_name}", headerNameWithoutExt)
                                   .Replace("{HEADER_NAME}", headerNameUpper)
                                   .Replace("{KEY_BLOCK_DATA_ADDRESS}", $"0x{blockTopAddress.Item1:X8}")
                                   .Replace("{KEY_BLOCK_DATA_MIRROR_ADDRESS}", $"0x{blockTopAddress.Item2:X8}");

            string sourceText = Resources.key_data_source_template;
            string sourceNameWithoutExt = Path.GetFileNameWithoutExtension(sourcePath);
            string sourceNameUpper = sourceNameWithoutExt.ToUpper();
            sourceText = sourceText.Replace("{source_name}", sourceNameWithoutExt)
                                   .Replace("{header_name}", headerNameWithoutExt)
                                   .Replace("{ENCRYPTED_SESSION_KEY_DATA}", strEncryptedSessionKey)
                                   .Replace("{IV_DATA}", strIV)
                                   .Replace("{SHA1_HASH_DATA}", strSHA1Hash);

            return new Tuple<string, string>(headerText, sourceText);
        }

        /// <summary>
        /// convert byte array to source text
        /// </summary>
        /// <param name="byteArray"></param>
        /// <param name="indent"></param>
        /// <returns></returns>
        private string byteArrayToSourceText(byte[] byteArray, int indent)
        {
            string strIndent = new string(' ', indent);
            string strInnerIndent = new string(' ', 4);

            string script = $"{strIndent}{{\r\n";
            for (int i = 0; i < byteArray.Length; i++)
            {
                if (i % 16 == 0)
                {
                    script += strIndent + strInnerIndent;
                }
                script += $"0x{BitConverter.ToString(byteArray, i, 1)}";
                if (i % 16 == 15)
                {
                    script += ",\r\n";
                }
                else
                {
                    script += ", ";
                }
            }
            if (script.EndsWith("\r\n"))
            {
                script = script.Remove(script.Length - 3, 1); // remove last comma
            }
            else // ends with ", "
            {
                script = script.Remove(script.Length - 2, 2) + "\r\n"; // remove last comma
            }
            script += $"{strIndent}}},";

            return script;
        }

        /// <summary>
        /// encrypt key data
        /// </summary>
        /// <param name="keyData"></param>
        /// <param name="keyCBC"></param>
        /// <param name="keyCBCMAC"></param>
        /// <param name="IV"></param>
        /// <returns></returns>
        private byte[] encryptKeyData(byte[] keyData, byte[] keyCBC, byte[] keyCBCMAC, byte[] IV)
        {
            byte[] outputCipher = new byte[keyData.Length + IV_MAC_BYTE_SIZE];

            // Create AesCryptoServiceProvider Object
            AesCryptoServiceProvider aes = new AesCryptoServiceProvider();
            // Set aes propery
            aes.BlockSize = 128;
            aes.Mode = CipherMode.ECB;
            aes.Padding = PaddingMode.None;
            // Set Aes key size
            aes.KeySize = 128;

            // Create AES encryption object
            aes.Key = keyCBC;
            ICryptoTransform encryptCBC = aes.CreateEncryptor();
            aes.Key = keyCBCMAC;
            ICryptoTransform encryptCBCMAC = aes.CreateEncryptor();

            //Create MemoryStream and CryptoStream
            using (MemoryStream msCBC = new MemoryStream())
            using (MemoryStream msCBCMAC = new MemoryStream())
            using (CryptoStream csCbc = new CryptoStream(msCBC, encryptCBC, CryptoStreamMode.Write))
            using (CryptoStream csCbcMac = new CryptoStream(msCBCMAC, encryptCBCMAC, CryptoStreamMode.Write))
            {
                byte[] iv = new byte[IV_MAC_BYTE_SIZE];
                byte[] mac = new byte[IV_MAC_BYTE_SIZE];
                byte[] instData = new byte[IV_MAC_BYTE_SIZE];

                Array.Copy(IV, iv, IV_MAC_BYTE_SIZE);
                Array.Clear(mac, 0, mac.Length);

                for (int i = 0; i < keyData.Length; i += IV_MAC_BYTE_SIZE)
                {
                    for (int j = 0; j < IV_MAC_BYTE_SIZE; j++)
                    {
                        instData[j] = Convert.ToByte(keyData[i + j] ^ mac[j]);
                        csCbcMac.Write(instData, j, 1);  // aes128 encrypt using input_cbc_mac_key
                    }
                    mac = msCBCMAC.GetBuffer();

                    for (int j = 0; j < IV_MAC_BYTE_SIZE; j++)
                    {
                        instData[j] = Convert.ToByte(keyData[i + j] ^ iv[j]);
                        csCbc.Write(instData, j, 1);  // aes128 encrypt using input_cbc_key
                    }
                    instData = msCBC.GetBuffer();

                    for (int j = 0; j < IV_MAC_BYTE_SIZE; j++)
                    {
                        outputCipher[i + j] = instData[j];
                        iv[j] = instData[j];
                    }
                    msCBC.Seek(0, 0);
                    msCBCMAC.Seek(0, 0);
                }

                /* output encrypted mac */
                for (int j = 0; j < IV_MAC_BYTE_SIZE; j++)
                {
                    instData[j] = Convert.ToByte(mac[j] ^ iv[j]);
                    csCbc.Write(instData, j, 1);  // aes128 encrypt using input_cbc_key
                }
                instData = msCBC.GetBuffer();
                for (int j = 0; j < IV_MAC_BYTE_SIZE; j++)
                {
                    outputCipher[keyData.Length + j] = instData[j];
                }

            }

            aes.Dispose();
            encryptCBC.Dispose();
            encryptCBCMAC.Dispose();

            return outputCipher;
        }

        /// <summary>
        /// get data block/data block mirror top addresses 
        /// </summary>
        /// <param name="mcu"></param>
        /// <returns></returns>
        private Tuple<uint, uint> getDataBlockStartAddress(Mcu mcu)
        {
            uint blockTopAddress = 0x00000000;
            uint blockMirrorTopAddress = 0x00000000;
            uint offset = 0;
            switch (mcu)
            {
                case Mcu.RX231:
                    blockTopAddress = McuSpecs[MCUROM_RX231_512K_SB_32KB].dataFlashTopAddress;
                    offset = (McuSpecs[MCUROM_RX231_512K_SB_32KB].dataFlashBottomAddress - blockTopAddress + 1) / 2;
                    blockMirrorTopAddress = blockTopAddress + offset;
                    break;
                case Mcu.RX65N:
                    blockTopAddress = McuSpecs[MCUROM_RX65N_2M_SB_64KB].dataFlashTopAddress;
                    offset = (McuSpecs[MCUROM_RX65N_2M_SB_64KB].dataFlashBottomAddress - blockTopAddress + 1) / 2;
                    blockMirrorTopAddress = blockTopAddress + offset;
                    break;
                case Mcu.RX66T:
                    blockTopAddress = McuSpecs[MCUROM_RX66T_512K_SB_64KB].dataFlashTopAddress;
                    offset = (McuSpecs[MCUROM_RX66T_512K_SB_64KB].dataFlashBottomAddress - blockTopAddress + 1) / 2;
                    blockMirrorTopAddress = blockTopAddress + offset;
                    break;
                case Mcu.RX72T:
                    blockTopAddress = McuSpecs[MCUROM_RX72T_1M_SB_64KB].dataFlashTopAddress;
                    offset = (McuSpecs[MCUROM_RX72T_1M_SB_64KB].dataFlashBottomAddress - blockTopAddress + 1) / 2;
                    blockMirrorTopAddress = blockTopAddress + offset;
                    break;
                default:
                    break;
            }

            return new Tuple<uint, uint>(blockTopAddress, blockMirrorTopAddress);
        }

		#endregion [Key Wrap] Tab

		#region [Firm Update] Tab

		/// <summary>
		/// [Firm Update] Tab - [Browse...] button of [User Private Key Path]
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		/*private void buttonBrowseUserPrivateKey_Click(object sender, EventArgs e)
		{
			openFileDialog.Filter = "User Private Key File|*.privatekey";
			openFileDialog.Title = "Specify the User Private Key File Name";
			openFileDialog.FileName = "";

			if (openFileDialog.ShowDialog() != DialogResult.OK || openFileDialog.FileName == "")
			{
				print_log("please specify the user private key file name.");
				return;
			}

			textBoxUserPrivateKeyPath.Text = openFileDialog.FileName;
		}*/

		/// <summary>
		/// [Firm Update] Tab - [Browse...] button of [User Program File Path]
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void buttonBrowseUserprog_Click(object sender, EventArgs e)
        {
            // Displays a OpenFileDialog so the user can save the Image
            openFileDialog.Filter = "Motorola Format File|*.mot";
            openFileDialog.Title = "Open the Motorola Format File";
            openFileDialog.FileName = "";

            if (openFileDialog.ShowDialog() != DialogResult.OK || openFileDialog.FileName == "")
            {
                print_log("please specify the motorola file name.");
                return;
            }

            textBoxUserProgramFilePath.Text = openFileDialog.FileName;
        }

        private void GenerateUserprog(string mcuName)
        {
            uint user_program_top_address = McuSpecs[mcuName].userProgramTopAddress;
            uint user_program_bottom_address = McuSpecs[mcuName].userProgramBottomAddress; ;
            uint code_flash_top_address = McuSpecs[mcuName].codeFlashTopAddress;
            uint code_flash_bottom_address = McuSpecs[mcuName].codeFlashBottomAddress;
            uint user_program_const_data_top_address = McuSpecs[mcuName].userProgramConstDataTopAddress;
            uint user_program_const_data_bottom_address = McuSpecs[mcuName].userProgramConstDataBottomAddress; ;
            uint data_flash_top_address = McuSpecs[mcuName].dataFlashTopAddress;
            uint data_flash_bottom_address = McuSpecs[mcuName].dataFlashBottomAddress;


            string[] block_buf = new string[32];

            byte[] userProgramKey = convertStrDataToKeyData(textBoxUserProgramKey_Aes128.Text, USER_PROGRAM_KEY_BYTE_SIZE);

            byte[] code_flash_image = new byte[1024 * 1024 * 4];  // 4MB image
            for (int i = 0; i < code_flash_image.Length; i++)
            {
                code_flash_image[i] = 0xff;
            }

            byte[] data_flash_image = new byte[1024 * 64];  // 64KB image
            for (int i = 0; i < data_flash_image.Length; i++)
            {
                data_flash_image[i] = 0xff;
            }
            //Set Aes key size
            aesCryptoProvider.KeySize = 128;

            byte[] iv = new byte[16];
            byte[] iv_init = new byte[16];
            byte[] tmpCBCKey = new byte[16];
            byte[] tmpCBCMACKey = new byte[16];

            RNGCryptoServiceProvider rng = new RNGCryptoServiceProvider();
            rng.GetBytes(iv);
            rng.GetBytes(tmpCBCKey);
            rng.GetBytes(tmpCBCMACKey);

            for (int i = 0; i < aesCryptoProvider.BlockSize / 8; i++)
            {
                iv_init[i] = iv[i];
            }

            //Create AES encryption object
            aesCryptoProvider.Key = tmpCBCKey;
            ICryptoTransform encrypt1 = aesCryptoProvider.CreateEncryptor();
            aesCryptoProvider.Key = tmpCBCMACKey;
            ICryptoTransform encrypt2 = aesCryptoProvider.CreateEncryptor();
            aesCryptoProvider.Key = userProgramKey;
            ICryptoTransform encrypt3 = aesCryptoProvider.CreateEncryptor();
            aesCryptoProvider.Key = tmpCBCKey;
            ICryptoTransform encrypt4 = aesCryptoProvider.CreateEncryptor();

            //Create MemoryStream
            MemoryStream ms1 = new MemoryStream();
            MemoryStream ms2 = new MemoryStream();
            MemoryStream ms3 = new MemoryStream();
            MemoryStream ms4 = new MemoryStream();

            //Check File lock
            int file_check_cnt = 0;
            while (IsFileLocked(saveFileDialog.FileName))
            {
                if (file_check_cnt==0)
                {
                    Console.WriteLine("...Checking File Lock...\r\n");
                }
                file_check_cnt++;
            }
            file_check_cnt= 0;

            //Create CryptoStream
            using (CryptoStream cs1 = new CryptoStream(ms1, encrypt1, CryptoStreamMode.Write))
            using (CryptoStream cs2 = new CryptoStream(ms2, encrypt2, CryptoStreamMode.Write))
            using (CryptoStream cs3 = new CryptoStream(ms3, encrypt3, CryptoStreamMode.Write))
            using (CryptoStream cs4 = new CryptoStream(ms4, encrypt4, CryptoStreamMode.Write))
            using (StreamReader sr = new StreamReader(textBoxUserProgramFilePath.Text))
            using (BinaryWriter bw = new BinaryWriter(File.Open(saveFileDialog.FileName, FileMode.Create)))
            {
                uint current_user_firm_address = 0;
                int total_length = 0;
                byte[] data;

                while (true)
                {
                    string line = sr.ReadLine();
                    if (line == null)
                    {
                        break;
                    }

                    string[] line_buf = new string[16];

                    line_buf[0] = line.Substring(0, 2);    // type field
                    line_buf[1] = line.Substring(2, 2);    // length

                    switch (line_buf[0])
                    {
                        case "S0":
                            line_buf[2] = line.Substring(4, 4);                 // zero
                            line_buf[3] = line.Substring(8, line.Length - 8);   // comment
                            break;
                        case "S1":
                            line_buf[2] = line.Substring(4, 4);                 // address
                            line_buf[3] = line.Substring(8, line.Length - 8);   // comment
                            break;
                        case "S2":
                            line_buf[2] = line.Substring(4, 6);                 // address
                            line_buf[3] = line.Substring(10, line.Length - 10); // comment
                            break;
                        case "S3":
                            line_buf[2] = line.Substring(4, 8);                 // address
                            line_buf[3] = line.Substring(12, line.Length - 12); // comment
                            break;
                        case "S4":
                            break;
                        case "S5":
                            line_buf[2] = line.Substring(4, 4);                 // recode number
                            break;
                        case "S6":
                            break;
                        case "S7":
                            break;
                    }

                    if ((line_buf[0] == "S3") || (line_buf[0] == "S2"))
                    {
                        int data_len;
                        if (line_buf[0] == "S3")
                        {
                            data_len = Convert.ToByte(line_buf[1], 16) - 5;     // -5 means: (address = 4 byte + checksum = 1 byte)
                        }
                        else
                        {
                            data_len = Convert.ToByte(line_buf[1], 16) - 4;     // -4 means: (address = 3 byte + checksum = 1 byte)
                        }

                        current_user_firm_address = Convert.ToUInt32(line_buf[2], 16);

                        if ((current_user_firm_address >= data_flash_top_address)
                            && (current_user_firm_address <= data_flash_bottom_address))
                        {
                            if ((current_user_firm_address < user_program_const_data_top_address)
                                || (current_user_firm_address > user_program_const_data_bottom_address))
                            {
                                print_log(String.Format("your motorola file includes prohibited address 0x{0:x08} on data flash, out of 0x{1:x08}-0x{2:x08}.\r\n", current_user_firm_address, user_program_const_data_top_address, user_program_const_data_bottom_address));
                                return;
                            }
                            uint offset = Convert.ToUInt32(line_buf[2], 16) - user_program_const_data_top_address;
                            for (int i = 0; (i / 2) < data_len; i += 2)
                            {
                                data_flash_image[(i / 2) + offset] = Convert.ToByte(line_buf[3].Substring(i, 2), 16);
                            }
                            current_user_firm_address = 0;
                            continue;
                        }

                        if ((current_user_firm_address >= code_flash_top_address)
                            && (current_user_firm_address <= code_flash_bottom_address))
                        {
                            if ((current_user_firm_address < user_program_top_address)
                                || (current_user_firm_address > (user_program_bottom_address + 1)))
                            {
                                print_log(String.Format("your motorola file includes prohibited address 0x{0:x08} on code flash, out of 0x{1:x08}-0x{2:x08}.\r\n", current_user_firm_address, user_program_top_address, user_program_bottom_address));
                                return;
                            }
                            uint offset = Convert.ToUInt32(line_buf[2], 16) - user_program_top_address;
                            for (int i = 0; (i / 2) < data_len; i += 2)
                            {
                                code_flash_image[(i / 2) + offset] = Convert.ToByte(line_buf[3].Substring(i, 2), 16);
                            }
                            total_length += data_len;
                            current_user_firm_address = 0;
                            continue;
                        }
                    }
                }

                if (comboBoxFirmwareVerificationType.Text == FIRMWARE_VERIFICATION_TYPE_MAC_AES128_CMAC_WITH_TSIP)
                {
                    // Execute encryption follow TSIP procedure
                    byte[] tmp = new byte[16];
                    byte[] UpProgram = new byte[16];
                    byte[] checksum = new byte[16];
                    byte[] SessionKey0 = new byte[16];
                    byte[] SessionKey1 = new byte[16];

                    for (int i = 0; i < (user_program_bottom_address + 1) - user_program_top_address; i += (aesCryptoProvider.BlockSize / 8))
                    {
                        for (int j = 0; j < aesCryptoProvider.BlockSize / 8; j++)
                        {
                            checksum[j] = Convert.ToByte(code_flash_image[i + j] ^ checksum[j]);
                            UpProgram[j] = Convert.ToByte(code_flash_image[i + j] ^ iv[j]);
                        }
                        for (int j = 0; j < aesCryptoProvider.BlockSize / 8; j++)
                        {
                            cs2.Write(checksum, j, 1);  // encrypt using CBCMAC
                        }
                        tmp = ms2.GetBuffer();
                        for (int j = 0; j < aesCryptoProvider.BlockSize / 8; j++)
                        {
                            checksum[j] = tmp[i + j];
                        }
                        for (int j = 0; j < aesCryptoProvider.BlockSize / 8; j++)
                        {
                            cs1.Write(UpProgram, j, 1);  // encrypt using CBC
                        }
                        tmp = ms1.GetBuffer();
                        for (int j = 0; j < aesCryptoProvider.BlockSize / 8; j++)
                        {
                            UpProgram[j] = tmp[i + j];
                        }
                        for (int j = 0; j < aesCryptoProvider.BlockSize / 8; j++)
                        {
                            iv[j] = UpProgram[j];
                        }
                    }
                    for (int i = 0; i < aesCryptoProvider.BlockSize / 8; i++)
                    {
                        checksum[i] = Convert.ToByte(iv[i] ^ checksum[i]);
                    }
                    cs4.Write(checksum, 0, aesCryptoProvider.BlockSize / 8);  // encrypt using CBCMAC
                    tmp = ms4.GetBuffer();
                    for (int i = 0; i < aesCryptoProvider.BlockSize / 8; i++)
                    {
                        checksum[i] = tmp[i];
                    }

                    cs3.Write(tmpCBCKey, 0, aesCryptoProvider.BlockSize / 8);  // encrypt using user_program_key
                    cs3.Write(tmpCBCMACKey, 0, aesCryptoProvider.BlockSize / 8);
                    tmp = ms3.GetBuffer();
                    for (int i = 0; i < 2; i++)
                    {
                        for (int j = 0; j < aesCryptoProvider.BlockSize / 8; j++)
                        {
                            if (i == 0)
                            {
                                SessionKey0[j] = tmp[(i * (aesCryptoProvider.BlockSize / 8)) + j];
                            }
                            else
                            {
                                SessionKey1[j] = tmp[(i * (aesCryptoProvider.BlockSize / 8)) + j];
                            }
                        }
                    }

                    // Create pdate data①(iv, sessionkey0, sessionkey1, max_cnt, checksum)
                    string iv_base64 = Convert.ToBase64String(iv_init, 0, 16);
                    string sessionkey0_base64 = Convert.ToBase64String(SessionKey0, 0, 16);
                    string sessionkey1_base64 = Convert.ToBase64String(SessionKey1, 0, 16);
                    string max_cnt = Convert.ToString((((user_program_bottom_address + 1) - user_program_top_address) / 4) + 4, 16); // +4 means for checksum
                    string checksum_base64 = Convert.ToBase64String(checksum, 0, 16);
                    string script;
                    script = $"iv {iv_base64}\r\n";
                    script += $"sessionkey0 {sessionkey0_base64}\r\n";
                    script += $"sessionkey1 {sessionkey1_base64}\r\n";
                    script += $"max_cnt {max_cnt}\r\n";
                    script += $"checksum {checksum_base64}\r\n";
                    data = System.Text.Encoding.ASCII.GetBytes(script);
                    bw.Write(data);

                    /* todo: upconst側と書き方を合わせる */
                    for (int i = 0; i < ms2.Length; i += 16)
                    {
                        string user_program_address = Convert.ToString(user_program_top_address + i, 16);
                        string user_program_base64 = Convert.ToBase64String(ms1.GetBuffer(), i, 16);

                        // Create pdate data②(upprogram)
                        script = $"upprogram {user_program_address} {user_program_base64}\r\n";
                        data = System.Text.Encoding.ASCII.GetBytes(script);
                        bw.Write(data);
                    }
                }
                else if ((comboBoxFirmwareVerificationType.Text == FIRMWARE_VERIFICATION_TYPE_HASH_SHA256) ||
                         (comboBoxFirmwareVerificationType.Text == FIRMWARE_VERIFICATION_TYPE_SIG_SHA256_ECDSA))
                {
                    //string script;
                    byte[] bs;
                    int hash_size;
                    string hash_value;
                    //string hash_string;

                    // prepair the rsu_header
                    rsu_header rsu_header_data = new rsu_header();
                    rsu_header_data.magic_code = System.Text.Encoding.ASCII.GetBytes("Renesas");
                    rsu_header_data.image_flag = IMAGE_FLAG_TESTING;
                    /*if (checkBox_CutOffForOTAData.Checked == false)
                    {
                        rsu_header_data.dataflash_flag = 1;
                        rsu_header_data.dataflash_start_address = McuSpecs[comboBoxMcu_firmupdate.Text].userProgramConstDataTopAddress;
                        rsu_header_data.dataflash_end_address = McuSpecs[comboBoxMcu_firmupdate.Text].userProgramConstDataBottomAddress;
                    }*/
                    rsu_header_data.sequence_number = Convert.ToUInt32(textBoxFirmwareSequenceNumber.Text);
                    rsu_header_data.start_address = McuSpecs[comboBoxMcu_firmupdate.Text].userProgramTopAddress;
                    rsu_header_data.end_address = McuSpecs[comboBoxMcu_firmupdate.Text].userProgramBottomAddress;
                    rsu_header_data.execution_address = McuSpecs[comboBoxMcu_firmupdate.Text].userProgramBottomAddress - 3;
                    rsu_header_data.hardware_id = McuSpecs[comboBoxMcu_firmupdate.Text].hardwareId;

                    // calculate hash
                    if (comboBoxFirmwareVerificationType.Text == FIRMWARE_VERIFICATION_TYPE_HASH_SHA256)
                    {
                        System.Security.Cryptography.SHA256CryptoServiceProvider sha_1 =
                        new System.Security.Cryptography.SHA256CryptoServiceProvider();

                        byte[] tmp = new byte[0];
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.sequence_number)).ToArray();
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.start_address)).ToArray();
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.end_address)).ToArray();
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.execution_address)).ToArray();
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.hardware_id)).ToArray();
                        tmp = tmp.Concat(rsu_header_data.reserved2).ToArray();
                        tmp = tmp.Concat(code_flash_image).ToArray();

                        int offset = CODE_FLASH_SIGNATURE_AREA_OFFSET;
                        int size = Convert.ToInt32((user_program_bottom_address + 1) - code_flash_top_address) - offset;
                        bs = sha_1.ComputeHash(tmp, 0, size);
                        sha_1.Clear();
                        hash_size = (sha_1.HashSize / 8);
                        hash_value = Convert.ToBase64String(bs, 0, hash_size);

                        Array.Copy(System.Text.Encoding.ASCII.GetBytes(comboBoxFirmwareVerificationType.Text), rsu_header_data.signature_type, comboBoxFirmwareVerificationType.Text.Length);
                        rsu_header_data.signature_size = (uint)hash_size;
                        Array.Copy(bs, rsu_header_data.signature, hash_size);
                    }
                    else if (comboBoxFirmwareVerificationType.Text == FIRMWARE_VERIFICATION_TYPE_HASH_SHA256)
                    {
                        System.Security.Cryptography.SHA256CryptoServiceProvider sha_256 =
                        new System.Security.Cryptography.SHA256CryptoServiceProvider();

                        byte[] tmp = new byte[0];
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.sequence_number)).ToArray();
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.start_address)).ToArray();
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.end_address)).ToArray();
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.execution_address)).ToArray();
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.hardware_id)).ToArray();
                        tmp = tmp.Concat(rsu_header_data.reserved2).ToArray();
                        tmp = tmp.Concat(code_flash_image).ToArray();

                        int offset = CODE_FLASH_SIGNATURE_AREA_OFFSET;
                        int size = Convert.ToInt32((user_program_bottom_address + 1) - code_flash_top_address) - offset;
                        bs = sha_256.ComputeHash(tmp, 0, size);
                        sha_256.Clear();
                        hash_size = (sha_256.HashSize / 8);
                        hash_value = Convert.ToBase64String(bs, 0, hash_size);

                        Array.Copy(System.Text.Encoding.ASCII.GetBytes(comboBoxFirmwareVerificationType.Text), rsu_header_data.signature_type, comboBoxFirmwareVerificationType.Text.Length);
                        rsu_header_data.signature_size = (uint)hash_size;
                        Array.Copy(bs, rsu_header_data.signature, hash_size);
                    }
                    else if (comboBoxFirmwareVerificationType.Text == FIRMWARE_VERIFICATION_TYPE_SIG_SHA256_ECDSA)
                    {
                        hash_value = "dummy"; // FIX ME. Necessary when entering a value in hash_string later. But unnecessary data for ECDSA signature.

                        byte[] tmp = new byte[0];
                        UInt32 Descriptor_address_size = 0x100;
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.sequence_number)).ToArray();
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.start_address)).ToArray();
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.end_address)).ToArray();
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.execution_address)).ToArray();
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.hardware_id)).ToArray();
                        tmp = tmp.Concat(rsu_header_data.reserved2).ToArray();
                        tmp = tmp.Concat(code_flash_image).ToArray();
                        Array.Resize(ref tmp, (Int32)(Descriptor_address_size + (user_program_bottom_address + 1) - user_program_top_address));

                        /*byte[] signature = Sign(tmp, textBoxUserPrivateKeyPath.Text);
                        bool result = Verify(tmp, signature, textBoxUserPrivateKeyPath.Text);
                        if (false == result)
                        {
                            print_log(String.Format("Failed to signature\r\n"));
                            return;
                        }*/
                        Array.Copy(System.Text.Encoding.ASCII.GetBytes(comboBoxFirmwareVerificationType.Text), rsu_header_data.signature_type, comboBoxFirmwareVerificationType.Text.Length);
                        //rsu_header_data.signature_size = (uint)signature.Length;
                        //Array.Copy(signature, rsu_header_data.signature, rsu_header_data.signature_size);
                    }
                    else
                    {
                        print_log(String.Format("This Firmware Verification Type is not implemented yet: [{0:s}]\r\n", comboBoxFirmwareVerificationType.Text));
                        return;
                    }
                    // CutOffForOTAData is always checked
                    //if (checkBox1_OutputBinaryFormat.Checked)
                    //{
                    /*if (!checkBox_CutOffForOTAData.Checked)
                    {
                        bw.Write(rsu_header_data.magic_code);
                        bw.Write(rsu_header_data.image_flag);
                        bw.Write(rsu_header_data.signature_type);
                        bw.Write(rsu_header_data.signature_size);
                        bw.Write(rsu_header_data.signature);
                        bw.Write(rsu_header_data.dataflash_flag);
                        bw.Write(rsu_header_data.dataflash_start_address);
                        bw.Write(rsu_header_data.dataflash_end_address);
                        bw.Write(rsu_header_data.reserved1);
                    }*/
                    bw.Write(rsu_header_data.sequence_number);
                    bw.Write(rsu_header_data.start_address);
                    bw.Write(rsu_header_data.end_address);
                    bw.Write(rsu_header_data.execution_address);
                    bw.Write(rsu_header_data.hardware_id);
                    bw.Write(rsu_header_data.reserved2);
                    bw.Write(code_flash_image, 0, (int)((user_program_bottom_address + 1) - user_program_top_address));
                    //}
                    /*else
                    {
                        hash_string = "sha256 ";
                        hash_string += hash_value;
                        hash_string += "\r\n";
                        data = System.Text.Encoding.ASCII.GetBytes(hash_string);
                        bw.Write(data);

                        for (int i = 0; i < ((user_program_bottom_address + 1) - user_program_top_address); i += 16)
                        {
                            string user_program_base64 = Convert.ToBase64String(code_flash_image, i, 16);

                            script = "upprogram ";
                            script += Convert.ToString(user_program_top_address + i, 16);
                            script += " ";
                            script += user_program_base64;
                            script += "\r\n";
                            data = System.Text.Encoding.ASCII.GetBytes(script);
                            bw.Write(data);
                        }
                    }*/
                }
                else
                {
                    /* not implemented yet */
                    print_log(String.Format("This Firmware Verification Type is not implemented yet: [{0:s}]\r\n", comboBoxFirmwareVerificationType.Text));
                    return;
                }

                // OTA check is done
                /*if (!checkBox_CutOffForOTAData.Checked)
                {
                    if (checkBox1_OutputBinaryFormat.Checked)
                    {
                        bw.Write(data_flash_image, 0, (int)(user_program_const_data_bottom_address - user_program_const_data_top_address) + 1);
                    }
                    else
                    {
                        for (int i = 0; i < (user_program_const_data_bottom_address - user_program_const_data_top_address) + 1; i += 16)
                        {
                            string script;
                            string user_program_base64 = Convert.ToBase64String(data_flash_image, i, 16);
                            script = "upconst ";
                            script += Convert.ToString(user_program_const_data_top_address + i, 16);
                            script += " ";
                            script += user_program_base64;
                            script += "\r\n";
                            data = System.Text.Encoding.ASCII.GetBytes(script);
                            bw.Write(data);
                        }
                    }
                }*/

            }
            print_log("generate succeeded.");

        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void buttonGenerateUserprog_Click(object sender, EventArgs e)
        {
            try
            {
                // check MCU name selection
                if (comboBoxMcu_firmupdate.SelectedIndex < 0)
                {
                    print_log("please select MCU in settings.");
                    return;
                }
                if (comboBoxFirmwareVerificationType.Text == "mac-aes128-cmac-with-tsip")
                {
                    // check user program key length
                    if (textBoxUserProgramKey_Aes128.Text.Length != 32)
                    {
                        print_log("please specify the correct key size.");
                        return;
                    }
                }
                // check combination of Firmware Verification Type x Use Code signing for AWS IoT

                // check user proguram file path
                if (!File.Exists(textBoxUserProgramFilePath.Text))
                {
                    print_log("please specify the motorola file name.");
                    return;
                }

				/*if (comboBoxFirmwareVerificationType.Text == "sig-sha256-ecdsa-standalone")
				{
					// check user private key path
					if (!File.Exists(textBoxUserPrivateKeyPath.Text))
					{
						print_log("please specify the user private key name.");
						return;
					}
				}*/

				// Displays a SaveFileDialog so the user can save
				saveFileDialog.Filter = "Renesas Secure Update|*.rsu";
                saveFileDialog.Title = "Save an (Encrypted(option)) User Program File";
                saveFileDialog.FileName = "userprog.rsu";

                if (saveFileDialog.ShowDialog() != DialogResult.OK || saveFileDialog.FileName == "")
                {
                    print_log("please specify the output file name.");
                    return;
                }

                // Convert user userprogram key data string to binary
                string mcuName = comboBoxMcu_firmupdate.Text;

                GenerateUserprog(mcuName);

                print_log("generate succeeded.");
            }
            catch (Exception)
            {
                print_log("exception has occurred.");
            }
        }

#endregion [Firm Update] Tab

        /// <summary>
        /// convert hex string to byte array
        /// </summary>
        /// <param name="strByteData"></param>
        /// <returns>byte array</returns>
        private byte[] convertStrDataToKeyData(string strByteData, int keyLength)
        {
            Debug.Assert(strByteData.Length / 2 <= keyLength);

            byte[] keyData = new byte[keyLength];
            Array.Clear(keyData, 0, keyData.Length);

            for (int i = 0; i < strByteData.Length; i += 2)
            {
                keyData[i / 2] = Convert.ToByte(strByteData.Substring(i, 2), 16);
            }

            return keyData;
        }

        /// <summary>
        /// display log message
        /// </summary>
        /// <param name="str"></param>
        private void print_log(string str)
        {
            info.Text += $"{log_count++}: {str}\r\n";

            info.SelectionStart = info.Text.Length;
            info.Focus();
            info.ScrollToCaret();
        }

        /// <summary>
        /// convert mcu to TSIP funciton level
        /// </summary>
        /// <param name="mcu"></param>
        /// <returns></returns>
        private TSIPLevel getTSIPFunctionLevel(Mcu mcu)
        {
            switch (mcu)
            {
                case Mcu.RX231:
                case Mcu.RX66T:
                case Mcu.RX72T:
                    return TSIPLevel.Lite;
                case Mcu.RX65N:
                    return TSIPLevel.Full;
                default:
                    return TSIPLevel.Lite;
            }
        }

		private void comboBoxFirmwareVerificationType_SelectedIndexChanged_1(object sender, EventArgs e)
		{
			if (comboBoxFirmwareVerificationType.Text == FIRMWARE_VERIFICATION_TYPE_MAC_AES128_CMAC_WITH_TSIP)
			{
				textBoxUserProgramKey_Aes128.Enabled = true;
			}
			else
			{
				textBoxUserProgramKey_Aes128.Enabled = false;
			}

			/*if ((comboBoxFirmwareVerificationType.Text == FIRMWARE_VERIFICATION_TYPE_SIG_SHA256_ECDSA) ||
                (comboBoxFirmwareVerificationType.Text == FIRMWARE_VERIFICATION_TYPE_SIG_SHA256_ECDSA_WITH_TSIP))

            {
				textBoxUserPrivateKeyPath.Enabled = true;
			}
			else
			{
				textBoxUserPrivateKeyPath.Enabled = false;
			}*/
		}

		

		/// <summary>
		/// Signature
		/// </summary>
		static byte[] Sign(byte[] plain, string key)
		{
			// Read the key.
			AsymmetricCipherKeyPair pair = null;
			using (var stream = new StreamReader(key))
			{
				var reader = new PemReader(stream);
				pair = reader.ReadObject() as AsymmetricCipherKeyPair;
			}

            // Generate signature instance and signature.
            ECDsaSigner signer = new ECDsaSigner(new HMacDsaKCalculator(new Sha256Digest()));
            signer.Init(true, pair.Private);
            SHA256 sha256 = new SHA256CryptoServiceProvider();
            var hash = sha256.ComputeHash(plain);
			var sign = signer.GenerateSignature(hash);

			// Convert signature value to byte [].
            var sign1 = sign[0].ToByteArray().SkipWhile(b => b == 0x00);
            var sign2 = sign[1].ToByteArray().SkipWhile(b => b == 0x00);
            byte[] signature = sign1.Concat(sign2).ToArray();

			return signature;
		}

		/// <summary>
		/// Verify signature
		/// </summary>
		static bool Verify(byte[] plain, byte[] signature, string key)
		{
			// Read the key.
			AsymmetricCipherKeyPair pair = null;
			using (var stream = new StreamReader(key))
			{
				var reader = new PemReader(stream);
				pair = reader.ReadObject() as AsymmetricCipherKeyPair;
			}

			// Convert signature value to BigInteger.
            var sign1 = signature.Take(32).ToArray();
            if ((sign1[0] & 0x80) == 0x80) sign1 = new byte[] { 0x00 }.Concat(sign1).ToArray();
            var sign2 = signature.Skip(32).ToArray();
            if ((sign2[0] & 0x80) == 0x80) sign2 = new byte[] { 0x00 }.Concat(sign2).ToArray();
			var sign = new BigInteger[] { new BigInteger(sign1), new BigInteger(sign2) };

            // Verify signature.
            ECDsaSigner signer = new ECDsaSigner(new HMacDsaKCalculator(new Sha256Digest()));
            signer.Init(false, pair.Public);
            SHA256 sha256 = new SHA256CryptoServiceProvider();
            var hash = sha256.ComputeHash(plain);
            var result = signer.VerifySignature(hash, sign[0], sign[1]);

			return result;
		}

#region [Inital Firm] Tab

        /// <summary>
        /// [Firm Update] Tab - [Browse...] button of [User Private Key Path]
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        
        

        

        private void tabPage2_Click(object sender, EventArgs e)
        {

        }
        

        private void buttonBrowseInitialUserPrivateKey_Click(object sender, EventArgs e)
        {
            openFileDialog.Filter = "User Private Key File|*.privatekey";
            openFileDialog.Title = "Specify the User Private Key File Name";
            openFileDialog.FileName = "";

            if (openFileDialog.ShowDialog() != DialogResult.OK || openFileDialog.FileName == "")
            {
                print_log("please specify the user private key file name.");
                return;
            }

            textBoxInitialUserPrivateKeyPath.Text = openFileDialog.FileName;

        }
#endregion [Inital Firm] Tab

        //generate
        private void buttonBrowseInitialUserprog_Click(object sender, EventArgs e)
        {
            // Displays a OpenFileDialog so the user can save the Image
            openFileDialog.Filter = "Motorola Format File|*.mot";
            openFileDialog.Title = "Open the Motorola Format File";
            openFileDialog.FileName = "";

            if (openFileDialog.ShowDialog() != DialogResult.OK || openFileDialog.FileName == "")
            {
                print_log("please specify the motorola file name.");
                return;
            }

            textBoxInitialUserProgramFilePath.Text = openFileDialog.FileName;
        }

        

        //end generate
        private void comboBox2_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void comboBoxInitialFirmwareVerificationType_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboBoxInitialFirmwareVerificationType.Text == FIRMWARE_VERIFICATION_TYPE_MAC_AES128_CMAC_WITH_TSIP)
            {
                textBoxInitialUserProgramKey_Aes128.Enabled = true;
            }
            else
            {
                textBoxInitialUserProgramKey_Aes128.Enabled = false;
            }

            if ((comboBoxInitialFirmwareVerificationType.Text == FIRMWARE_VERIFICATION_TYPE_SIG_SHA256_ECDSA) ||
                (comboBoxInitialFirmwareVerificationType.Text == FIRMWARE_VERIFICATION_TYPE_SIG_SHA256_ECDSA_WITH_TSIP))

            {
                textBoxInitialUserPrivateKeyPath.Enabled = true;
            }
            else
            {
                textBoxInitialUserPrivateKeyPath.Enabled = false;
            }
        }

        private void GenerateInitialUserprog(string mcuName)
        {
            string check_sum;
            string s0_buf = "";
            int current_pointer;
            int next_pointer;
            string s2_s3_buf = "";
            string s3_header_buf = "";
            string s3_app_buf = "";
            string s3_boot_buf = "";

            /* ブートローダ読みだす。指定したパスから読み出せるようにする */
            StreamReader sr_boot_loader = new StreamReader(textBoxInitialBootLoaderUserProgramFilePath.Text, Encoding.GetEncoding("Shift_JIS"));
            string str_boot_loader = sr_boot_loader.ReadToEnd();
            sr_boot_loader.Close();

            /* ユーザアプリ読み出す。指定したパスから読み出せるようにする */
            //StreamReader sr_user_application = new StreamReader(textBoxUserProgramFilePath.Text, Encoding.GetEncoding("Shift_JIS"));
            StreamReader sr_user_application = new StreamReader(textBoxInitialUserProgramFilePath.Text, Encoding.GetEncoding("Shift_JIS"));
            string str_user_application = sr_user_application.ReadToEnd();
            sr_user_application.Close();

            /* S0: Initila_firm.mot */
            /* レコードマーク/タイプ: S0 */
            /* レコード長: 0F */
            /* アドレス: 0000 */
            /* データ: 6177735F746573742E6D6F74(userprog.mot) */
            /* チェックサム: check_sumの値 */
            current_pointer = 0;
            next_pointer = 0;
            check_sum = CalculateMotorolaChecksum(INITIAL_FIRM_MOT_S0_FORMAT);
            StringBuilder sb = new StringBuilder();
            sb.Append("S0");
            sb.Append(INITIAL_FIRM_MOT_S0_FORMAT);
            sb.Append(check_sum.PadLeft(2, '0'));
            sb.Append("\r\n");
            s0_buf = sb.ToString();

            /* S2: ユーザアプリ(データフラッシュ) + S3: オプション設定メモリ */
            current_pointer = str_user_application.IndexOf("S214100");
            next_pointer = str_user_application.IndexOf("S315FFF00300");
            s2_s3_buf = str_user_application.Substring(current_pointer, next_pointer - current_pointer);

            /* S3: ヘッダ情報(0xFFE00000 - 0xFFE002FF) */
            // Convert user userprogram key data string to binary
            uint user_program_top_address = McuSpecs[mcuName].userProgramTopAddress;
            uint user_program_bottom_address = McuSpecs[mcuName].userProgramBottomAddress; ;
            uint code_flash_top_address = McuSpecs[mcuName].codeFlashTopAddress;
            uint code_flash_bottom_address = McuSpecs[mcuName].codeFlashBottomAddress;
            uint user_program_const_data_top_address = McuSpecs[mcuName].userProgramConstDataTopAddress;
            uint user_program_const_data_bottom_address = McuSpecs[mcuName].userProgramConstDataBottomAddress; ;
            uint data_flash_top_address = McuSpecs[mcuName].dataFlashTopAddress;
            uint data_flash_bottom_address = McuSpecs[mcuName].dataFlashBottomAddress;

            string[] block_buf = new string[32];

            byte[] userProgramKey = convertStrDataToKeyData(textBoxInitialUserProgramKey_Aes128.Text, USER_PROGRAM_KEY_BYTE_SIZE);

            byte[] code_flash_image = new byte[1024 * 1024 * 4];  // 4MB image
            for (int i = 0; i < code_flash_image.Length; i++)
            {
                code_flash_image[i] = 0xff;
            }

            byte[] data_flash_image = new byte[1024 * 64];  // 64KB image
            for (int i = 0; i < data_flash_image.Length; i++)
            {
                data_flash_image[i] = 0xff;
            }
            //Set Aes key size
            aesCryptoProvider.KeySize = 128;

            byte[] iv = new byte[16];
            byte[] iv_init = new byte[16];
            byte[] tmpCBCKey = new byte[16];
            byte[] tmpCBCMACKey = new byte[16];

            RNGCryptoServiceProvider rng = new RNGCryptoServiceProvider();
            rng.GetBytes(iv);
            rng.GetBytes(tmpCBCKey);
            rng.GetBytes(tmpCBCMACKey);

            for (int i = 0; i < aesCryptoProvider.BlockSize / 8; i++)
            {
                iv_init[i] = iv[i];
            }

            //Create AES encryption object
            aesCryptoProvider.Key = tmpCBCKey;
            ICryptoTransform encrypt1 = aesCryptoProvider.CreateEncryptor();
            aesCryptoProvider.Key = tmpCBCMACKey;
            ICryptoTransform encrypt2 = aesCryptoProvider.CreateEncryptor();
            aesCryptoProvider.Key = userProgramKey;
            ICryptoTransform encrypt3 = aesCryptoProvider.CreateEncryptor();
            aesCryptoProvider.Key = tmpCBCKey;
            ICryptoTransform encrypt4 = aesCryptoProvider.CreateEncryptor();

            //Create MemoryStream
            MemoryStream ms1 = new MemoryStream();
            MemoryStream ms2 = new MemoryStream();
            MemoryStream ms3 = new MemoryStream();
            MemoryStream ms4 = new MemoryStream();

            rsu_header rsu_header_data = new rsu_header();

            //Check File lock
            int file_check_cnt = 0;
            while (IsFileLocked(saveFileDialog.FileName))
            {
                if (file_check_cnt == 0)
                {
                    Console.WriteLine("... Checking File Lock ...\r\n");
                }
                file_check_cnt++;
            }
            file_check_cnt = 0;

            //Create CryptoStream
            using (CryptoStream cs1 = new CryptoStream(ms1, encrypt1, CryptoStreamMode.Write))
            using (CryptoStream cs2 = new CryptoStream(ms2, encrypt2, CryptoStreamMode.Write))
            using (CryptoStream cs3 = new CryptoStream(ms3, encrypt3, CryptoStreamMode.Write))
            using (CryptoStream cs4 = new CryptoStream(ms4, encrypt4, CryptoStreamMode.Write))
            using (StreamReader sr = new StreamReader(textBoxInitialUserProgramFilePath.Text))
            using (BinaryWriter bw = new BinaryWriter(File.Open(saveFileDialog.FileName, FileMode.Create)))
            {
                uint current_user_firm_address = 0;
                int total_length = 0;
                byte[] data;

                while (true)
                {
                    string line = sr.ReadLine();
                    if (line == null)
                    {
                        break;
                    }

                    string[] line_buf = new string[16];

                    line_buf[0] = line.Substring(0, 2);    // type field
                    line_buf[1] = line.Substring(2, 2);    // length

                    switch (line_buf[0])
                    {
                        case "S0":
                            line_buf[2] = line.Substring(4, 4);                 // zero
                            line_buf[3] = line.Substring(8, line.Length - 8);   // comment
                            break;
                        case "S1":
                            line_buf[2] = line.Substring(4, 4);                 // address
                            line_buf[3] = line.Substring(8, line.Length - 8);   // comment
                            break;
                        case "S2":
                            line_buf[2] = line.Substring(4, 6);                 // address
                            line_buf[3] = line.Substring(10, line.Length - 10); // comment
                            break;
                        case "S3":
                            line_buf[2] = line.Substring(4, 8);                 // address
                            line_buf[3] = line.Substring(12, line.Length - 12); // comment
                            break;
                        case "S4":
                            break;
                        case "S5":
                            line_buf[2] = line.Substring(4, 4);                 // recode number
                            break;
                        case "S6":
                            break;
                        case "S7":
                            break;
                    }

                    if ((line_buf[0] == "S3") || (line_buf[0] == "S2"))
                    {
                        int data_len;
                        if (line_buf[0] == "S3")
                        {
                            data_len = Convert.ToByte(line_buf[1], 16) - 5;     // -5 means: (address = 4 byte + checksum = 1 byte)
                        }
                        else
                        {
                            data_len = Convert.ToByte(line_buf[1], 16) - 4;     // -4 means: (address = 3 byte + checksum = 1 byte)
                        }

                        current_user_firm_address = Convert.ToUInt32(line_buf[2], 16);

                        if ((current_user_firm_address >= data_flash_top_address)
                            && (current_user_firm_address <= data_flash_bottom_address))
                        {
                            if ((current_user_firm_address < user_program_const_data_top_address)
                                || (current_user_firm_address > user_program_const_data_bottom_address))
                            {
                                print_log(String.Format("your motorola file includes prohibited address 0x{0:x08} on data flash, out of 0x{1:x08}-0x{2:x08}.\r\n", current_user_firm_address, user_program_const_data_top_address, user_program_const_data_bottom_address));
                                return;
                            }
                            uint offset = Convert.ToUInt32(line_buf[2], 16) - user_program_const_data_top_address;
                            for (int i = 0; (i / 2) < data_len; i += 2)
                            {
                                data_flash_image[(i / 2) + offset] = Convert.ToByte(line_buf[3].Substring(i, 2), 16);
                            }
                            current_user_firm_address = 0;
                            continue;
                        }

                        if ((current_user_firm_address >= code_flash_top_address)
                            && (current_user_firm_address <= code_flash_bottom_address))
                        {
                            if ((current_user_firm_address < user_program_top_address)
                                || (current_user_firm_address > (user_program_bottom_address + 1)))
                            {
                                print_log(String.Format("your motorola file includes prohibited address 0x{0:x08} on code flash, out of 0x{1:x08}-0x{2:x08}.\r\n", current_user_firm_address, user_program_top_address, user_program_bottom_address));
                                return;
                            }
                            uint offset = Convert.ToUInt32(line_buf[2], 16) - user_program_top_address;
                            for (int i = 0; (i / 2) < data_len; i += 2)
                            {
                                code_flash_image[(i / 2) + offset] = Convert.ToByte(line_buf[3].Substring(i, 2), 16);
                            }
                            total_length += data_len;
                            current_user_firm_address = 0;
                            continue;
                        }
                    }
                }

                if (comboBoxInitialFirmwareVerificationType.Text == FIRMWARE_VERIFICATION_TYPE_MAC_AES128_CMAC_WITH_TSIP)
                {
                    // Execute encryption follow TSIP procedure
                    byte[] tmp = new byte[16];
                    byte[] UpProgram = new byte[16];
                    byte[] checksum = new byte[16];
                    byte[] SessionKey0 = new byte[16];
                    byte[] SessionKey1 = new byte[16];

                    for (int i = 0; i < (user_program_bottom_address + 1) - user_program_top_address; i += (aesCryptoProvider.BlockSize / 8))
                    {
                        for (int j = 0; j < aesCryptoProvider.BlockSize / 8; j++)
                        {
                            checksum[j] = Convert.ToByte(code_flash_image[i + j] ^ checksum[j]);
                            UpProgram[j] = Convert.ToByte(code_flash_image[i + j] ^ iv[j]);
                        }
                        for (int j = 0; j < aesCryptoProvider.BlockSize / 8; j++)
                        {
                            cs2.Write(checksum, j, 1);  // encrypt using CBCMAC
                        }
                        tmp = ms2.GetBuffer();
                        for (int j = 0; j < aesCryptoProvider.BlockSize / 8; j++)
                        {
                            checksum[j] = tmp[i + j];
                        }
                        for (int j = 0; j < aesCryptoProvider.BlockSize / 8; j++)
                        {
                            cs1.Write(UpProgram, j, 1);  // encrypt using CBC
                        }
                        tmp = ms1.GetBuffer();
                        for (int j = 0; j < aesCryptoProvider.BlockSize / 8; j++)
                        {
                            UpProgram[j] = tmp[i + j];
                        }
                        for (int j = 0; j < aesCryptoProvider.BlockSize / 8; j++)
                        {
                            iv[j] = UpProgram[j];
                        }
                    }
                    for (int i = 0; i < aesCryptoProvider.BlockSize / 8; i++)
                    {
                        checksum[i] = Convert.ToByte(iv[i] ^ checksum[i]);
                    }
                    cs4.Write(checksum, 0, aesCryptoProvider.BlockSize / 8);  // encrypt using CBCMAC
                    tmp = ms4.GetBuffer();
                    for (int i = 0; i < aesCryptoProvider.BlockSize / 8; i++)
                    {
                        checksum[i] = tmp[i];
                    }

                    cs3.Write(tmpCBCKey, 0, aesCryptoProvider.BlockSize / 8);  // encrypt using user_program_key
                    cs3.Write(tmpCBCMACKey, 0, aesCryptoProvider.BlockSize / 8);
                    tmp = ms3.GetBuffer();
                    for (int i = 0; i < 2; i++)
                    {
                        for (int j = 0; j < aesCryptoProvider.BlockSize / 8; j++)
                        {
                            if (i == 0)
                            {
                                SessionKey0[j] = tmp[(i * (aesCryptoProvider.BlockSize / 8)) + j];
                            }
                            else
                            {
                                SessionKey1[j] = tmp[(i * (aesCryptoProvider.BlockSize / 8)) + j];
                            }
                        }
                    }

                    // Create pdate data①(iv, sessionkey0, sessionkey1, max_cnt, checksum)
                    string iv_base64 = Convert.ToBase64String(iv_init, 0, 16);
                    string sessionkey0_base64 = Convert.ToBase64String(SessionKey0, 0, 16);
                    string sessionkey1_base64 = Convert.ToBase64String(SessionKey1, 0, 16);
                    string max_cnt = Convert.ToString((((user_program_bottom_address + 1) - user_program_top_address) / 4) + 4, 16); // +4 means for checksum
                    string checksum_base64 = Convert.ToBase64String(checksum, 0, 16);
                    string script;
                    script = $"iv {iv_base64}\r\n";
                    script += $"sessionkey0 {sessionkey0_base64}\r\n";
                    script += $"sessionkey1 {sessionkey1_base64}\r\n";
                    script += $"max_cnt {max_cnt}\r\n";
                    script += $"checksum {checksum_base64}\r\n";
                    data = System.Text.Encoding.ASCII.GetBytes(script);
                    bw.Write(data);

                    /* todo: upconst側と書き方を合わせる */
                    for (int i = 0; i < ms2.Length; i += 16)
                    {
                        string user_program_address = Convert.ToString(user_program_top_address + i, 16);
                        string user_program_base64 = Convert.ToBase64String(ms1.GetBuffer(), i, 16);

                        // Create pdate data②(upprogram)
                        script = $"upprogram {user_program_address} {user_program_base64}\r\n";
                        data = System.Text.Encoding.ASCII.GetBytes(script);
                        bw.Write(data);
                    }
                }
                else if ((comboBoxInitialFirmwareVerificationType.Text == FIRMWARE_VERIFICATION_TYPE_HASH_SHA256) ||
                         (comboBoxInitialFirmwareVerificationType.Text == FIRMWARE_VERIFICATION_TYPE_SIG_SHA256_ECDSA))
                {
                    //string script;
                    byte[] bs;
                    int hash_size;
                    string hash_value;
                    //string hash_string;

                    // prepair the rsu_header
                    rsu_header_data.magic_code = System.Text.Encoding.ASCII.GetBytes("Renesas");
                    rsu_header_data.image_flag = IMAGE_FLAG_TESTING;

                    rsu_header_data.dataflash_flag = 1;
                    rsu_header_data.dataflash_start_address = McuSpecs[comboBox_Initial_Mcu_firmupdate.Text].userProgramConstDataTopAddress;
                    rsu_header_data.dataflash_end_address = McuSpecs[comboBox_Initial_Mcu_firmupdate.Text].userProgramConstDataBottomAddress;

                    rsu_header_data.sequence_number = Convert.ToUInt32(textBoxInitialFirmwareSequenceNumber.Text);
                    rsu_header_data.start_address = McuSpecs[comboBox_Initial_Mcu_firmupdate.Text].userProgramTopAddress;
                    rsu_header_data.end_address = McuSpecs[comboBox_Initial_Mcu_firmupdate.Text].userProgramBottomAddress;
                    rsu_header_data.execution_address = McuSpecs[comboBox_Initial_Mcu_firmupdate.Text].userProgramBottomAddress - 3;
                    rsu_header_data.hardware_id = McuSpecs[comboBox_Initial_Mcu_firmupdate.Text].hardwareId;

                    // calculate hash
                    if (comboBoxInitialFirmwareVerificationType.Text == FIRMWARE_VERIFICATION_TYPE_HASH_SHA256)
                    {
                        System.Security.Cryptography.SHA256CryptoServiceProvider sha_1 =
                        new System.Security.Cryptography.SHA256CryptoServiceProvider();

                        byte[] tmp = new byte[0];
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.sequence_number)).ToArray();
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.start_address)).ToArray();
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.end_address)).ToArray();
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.execution_address)).ToArray();
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.hardware_id)).ToArray();
                        tmp = tmp.Concat(rsu_header_data.reserved2).ToArray();
                        tmp = tmp.Concat(code_flash_image).ToArray();

                        int offset = CODE_FLASH_SIGNATURE_AREA_OFFSET;
                        int size = Convert.ToInt32((user_program_bottom_address + 1) - code_flash_top_address) - offset;
                        bs = sha_1.ComputeHash(tmp, 0, size);
                        sha_1.Clear();
                        hash_size = (sha_1.HashSize / 8);
                        hash_value = Convert.ToBase64String(bs, 0, hash_size);

                        Array.Copy(System.Text.Encoding.ASCII.GetBytes(comboBoxInitialFirmwareVerificationType.Text), rsu_header_data.signature_type, comboBoxInitialFirmwareVerificationType.Text.Length);
                        rsu_header_data.signature_size = (uint)hash_size;
                        Array.Copy(bs, rsu_header_data.signature, hash_size);
                    }
                    else if (comboBoxInitialFirmwareVerificationType.Text == FIRMWARE_VERIFICATION_TYPE_HASH_SHA256)
                    {
                        System.Security.Cryptography.SHA256CryptoServiceProvider sha_256 =
                        new System.Security.Cryptography.SHA256CryptoServiceProvider();

                        byte[] tmp = new byte[0];
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.sequence_number)).ToArray();
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.start_address)).ToArray();
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.end_address)).ToArray();
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.execution_address)).ToArray();
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.hardware_id)).ToArray();
                        tmp = tmp.Concat(rsu_header_data.reserved2).ToArray();
                        tmp = tmp.Concat(code_flash_image).ToArray();

                        int offset = CODE_FLASH_SIGNATURE_AREA_OFFSET;
                        int size = Convert.ToInt32((user_program_bottom_address + 1) - code_flash_top_address) - offset;
                        bs = sha_256.ComputeHash(tmp, 0, size);
                        sha_256.Clear();
                        hash_size = (sha_256.HashSize / 8);
                        hash_value = Convert.ToBase64String(bs, 0, hash_size);

                        Array.Copy(System.Text.Encoding.ASCII.GetBytes(comboBoxInitialFirmwareVerificationType.Text), rsu_header_data.signature_type, comboBoxInitialFirmwareVerificationType.Text.Length);
                        rsu_header_data.signature_size = (uint)hash_size;
                        Array.Copy(bs, rsu_header_data.signature, hash_size);
                    }
                    else if (comboBoxInitialFirmwareVerificationType.Text == FIRMWARE_VERIFICATION_TYPE_SIG_SHA256_ECDSA)
                    {
                        hash_value = "dummy"; // FIX ME. Necessary when entering a value in hash_string later. But unnecessary data for ECDSA signature.

                        byte[] tmp = new byte[0];
                        UInt32 Descriptor_address_size = 0x100;
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.sequence_number)).ToArray();
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.start_address)).ToArray();
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.end_address)).ToArray();
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.execution_address)).ToArray();
                        tmp = tmp.Concat(BitConverter.GetBytes(rsu_header_data.hardware_id)).ToArray();
                        tmp = tmp.Concat(rsu_header_data.reserved2).ToArray();
                        tmp = tmp.Concat(code_flash_image).ToArray();
                        Array.Resize(ref tmp, (Int32)(Descriptor_address_size + (user_program_bottom_address + 1) - user_program_top_address));

                        byte[] signature = Sign(tmp, textBoxInitialUserPrivateKeyPath.Text);
                        bool result = Verify(tmp, signature, textBoxInitialUserPrivateKeyPath.Text);
                        if (false == result)
                        {
                            print_log(String.Format("Failed to signature\r\n"));
                            return;
                        }
                        Array.Copy(System.Text.Encoding.ASCII.GetBytes(comboBoxInitialFirmwareVerificationType.Text), rsu_header_data.signature_type, comboBoxInitialFirmwareVerificationType.Text.Length);
                        rsu_header_data.signature_size = (uint)signature.Length;
                        Array.Copy(signature, rsu_header_data.signature, rsu_header_data.signature_size);
                    }
                    else
                    {
                        print_log(String.Format("This Firmware Verification Type is not implemented yet: [{0:s}]\r\n", comboBoxInitialFirmwareVerificationType.Text));
                        return;
                    }

                    if (checkBox1_InitialOutputBinaryFormat.Checked)
                    {
                        bw.Write(rsu_header_data.magic_code);
                        bw.Write(rsu_header_data.image_flag);
                        bw.Write(rsu_header_data.signature_type);
                        bw.Write(rsu_header_data.signature_size);
                        bw.Write(rsu_header_data.signature);
                        bw.Write(rsu_header_data.dataflash_flag);
                        bw.Write(rsu_header_data.dataflash_start_address);
                        bw.Write(rsu_header_data.dataflash_end_address);
                        bw.Write(rsu_header_data.reserved1);
                        bw.Write(rsu_header_data.sequence_number);
                        bw.Write(rsu_header_data.start_address);
                        bw.Write(rsu_header_data.end_address);
                        bw.Write(rsu_header_data.execution_address);
                        bw.Write(rsu_header_data.hardware_id);
                        bw.Write(rsu_header_data.reserved2);
                        bw.Write(code_flash_image, 0, (int)((user_program_bottom_address + 1) - user_program_top_address));
                        bw.Write(data_flash_image, 0, (int)((data_flash_bottom_address + 1) - data_flash_top_address));
                    }
                }
            }
            if ((checkBox1_InitialOutputBinaryFormat.Checked == false) && ((comboBoxInitialFirmwareVerificationType.Text == FIRMWARE_VERIFICATION_TYPE_HASH_SHA256) ||
                         (comboBoxInitialFirmwareVerificationType.Text == FIRMWARE_VERIFICATION_TYPE_SIG_SHA256_ECDSA)))
            {
                /* ヘッダ情報をbyte配列に詰める */
                byte[] s3_header_byte = new byte[0];
                byte[] s3_header_byte_tmp = new byte[1];
                s3_header_byte = s3_header_byte.Concat(rsu_header_data.magic_code).ToArray();
                s3_header_byte_tmp[0] = (byte)IMAGE_FLAG_INSTALLING;
                s3_header_byte = s3_header_byte.Concat(s3_header_byte_tmp).ToArray();
                s3_header_byte = s3_header_byte.Concat(rsu_header_data.signature_type).ToArray();
                s3_header_byte = s3_header_byte.Concat(BitConverter.GetBytes(rsu_header_data.signature_size)).ToArray();
                s3_header_byte = s3_header_byte.Concat(rsu_header_data.signature).ToArray();
                s3_header_byte = s3_header_byte.Concat(BitConverter.GetBytes(rsu_header_data.dataflash_flag)).ToArray();
                s3_header_byte = s3_header_byte.Concat(BitConverter.GetBytes(rsu_header_data.dataflash_start_address)).ToArray();
                s3_header_byte = s3_header_byte.Concat(BitConverter.GetBytes(rsu_header_data.dataflash_end_address)).ToArray();
                s3_header_byte = s3_header_byte.Concat(rsu_header_data.reserved1).ToArray();
                s3_header_byte = s3_header_byte.Concat(BitConverter.GetBytes(rsu_header_data.sequence_number)).ToArray();
                s3_header_byte = s3_header_byte.Concat(BitConverter.GetBytes(rsu_header_data.start_address)).ToArray();
                s3_header_byte = s3_header_byte.Concat(BitConverter.GetBytes(rsu_header_data.end_address)).ToArray();
                s3_header_byte = s3_header_byte.Concat(BitConverter.GetBytes(rsu_header_data.execution_address)).ToArray();
                s3_header_byte = s3_header_byte.Concat(BitConverter.GetBytes(rsu_header_data.hardware_id)).ToArray();
                s3_header_byte = s3_header_byte.Concat(rsu_header_data.reserved2).ToArray();

                /* byte配列から1ライン(16バイト)抽出し、S3フォーマットに成形する */
                // stringの先頭に"S315FFE00"を付与する
                // stringに"000"-"2F0"を後付けする
                // stringにs3_header_byteから16バイト抽出し、byte[]→stringに変換したデータを後付けする。
                // チェックサム関数に引数としてstirngを渡して、チェックサムを取得する
                // stringの最後にチェックサムを後付けする
                // これを30回繰り返す(FFE00000-FFE002F0)
                for (int i = 0; i < 0x300; i += 0x10)
                {
                    string s3_header_line = "S315FFE00";
                    s3_header_line = String.Concat(s3_header_line, i.ToString("X3"));
                    byte[] s3_header_line_tmp = new byte[16];
                    Buffer.BlockCopy(s3_header_byte, i, s3_header_line_tmp, 0, 0x10);
                    s3_header_line = String.Concat(s3_header_line, BitConverter.ToString(s3_header_line_tmp).Replace("-", string.Empty));
                    check_sum = CalculateMotorolaChecksum(s3_header_line.Substring(2, 42));
                    s3_header_line = String.Concat(s3_header_line, check_sum.PadLeft(2, '0'));
                    s3_header_line = String.Concat(s3_header_line, "\r\n");
                    s3_header_buf = String.Concat(s3_header_buf, s3_header_line);
                }
#if MYDEFINE
							byte[] s3_header_buf = new byte[0];
							s3_header_buf = s3_header_buf.Concat(rsu_header_data.magic_code).ToArray();
							var str_hex_tmp = BitConverter.GetBytes((char)IMAGE_FLAG_TESTING);
							

							s3_header_buf = s3_header_buf.Concat(rsu_header_data.signature_type).ToArray();
							s3_header_buf = s3_header_buf.Concat(BitConverter.GetBytes(rsu_header_data.sequence_number)).ToArray();
							s3_header_buf = s3_header_buf.Concat(BitConverter.GetBytes(rsu_header_data.start_address)).ToArray();
							s3_header_buf = s3_header_buf.Concat(BitConverter.GetBytes(rsu_header_data.end_address)).ToArray();
							s3_header_buf = s3_header_buf.Concat(BitConverter.GetBytes(rsu_header_data.execution_address)).ToArray();
							s3_header_buf = s3_header_buf.Concat(BitConverter.GetBytes(rsu_header_data.hardware_id)).ToArray();
							s3_header_buf = s3_header_buf.Concat(rsu_header_data.reserved2).ToArray();
#else

#endif

                /* S3: ユーザアプリ(0xFFE00300 - 0xFFEBFFFF) */
                /* アドレスに下駄はかせる 0xFFFxxxxx -> 0xFFExxxxx */

                /* 0xFFExxxxに変換して、再度チェックサムを付け直した値をs3_app_bufに格納する */
                // 1 line読み出す
                // S3、最終バイト(チェックサム)、\r\nを除いたstirngをチェックサム関数に渡す
                // チェックサムをReplaceする

                // すでにS315FFE00300のバイナリデータがcode_flash_image[]に入っている
                // "15FFE00300xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"に対して、チェックサムを計算する
                // S315FFE00300xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxXX\r\nをs3_app_bufに格納する
                // "15FFE00310xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"に対して、チェックサムを計算する
                // S315FFE00310xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxXX\r\nをs3_app_bufに格納する
                // ・・・
                // ‭785664‬バイトの処理が完了したら終了する

                string s3_app_buf_tmp = "";
                StringBuilder sb_app = new StringBuilder();
                StringBuilder sb_app_tmp = new StringBuilder();
                for (int i = 0; i < 785664; i += 16)
                {
                    int address = i + 0x00300;
                    sb_app_tmp.Append("15FFE");
                    sb_app_tmp.Append(address.ToString("X5"));
                    for (int j = 0; j < 16; j++)
                    {
                        sb_app_tmp.Append(code_flash_image[i + j].ToString("X2"));
                    }
                    s3_app_buf_tmp = sb_app_tmp.ToString();
                    sb_app_tmp.Clear();
                    check_sum = CalculateMotorolaChecksum(s3_app_buf_tmp);
                    sb_app.Append("S3");
                    sb_app.Append(s3_app_buf_tmp);
                    sb_app.Append(check_sum.PadLeft(2, '0'));
                    sb_app.Append("\r\n");
                }
                s3_app_buf = sb_app.ToString();
                sb_app.Clear();

                /* S3: ブートローダ(0xFFFC0000 - 0xFFFFFFFF) */
                current_pointer = str_boot_loader.IndexOf("S315FFFC0000");
                next_pointer = str_boot_loader.Length;
                s3_boot_buf = str_boot_loader.Substring(current_pointer, next_pointer - current_pointer);

                // 空の.motに「ブートローダ＋ユーザアプリ＋ヘッダ情報」を書き込み出力する。
                string total_buf = "";
                StringBuilder sb_total = new StringBuilder();
                sb_total.Append(s0_buf);
                sb_total.Append(s2_s3_buf);
                sb_total.Append(s3_header_buf);
                sb_total.Append(s3_app_buf);
                sb_total.Append(s3_boot_buf);
                total_buf = sb_total.ToString();
                File.WriteAllText(saveFileDialog.FileName, total_buf);
            }
        }

        private void buttonGenerateInitialUserprog(object sender, EventArgs e)
        {
            try
            {
                //string UserBootLoaderFilePath;

                // check MCU name selection
                if (comboBox_Initial_Mcu_firmupdate.SelectedIndex < 0)
                {
                    print_log("please select MCU in settings.");
                    return;
                }
                if (comboBoxInitialFirmwareVerificationType.Text == "mac-aes128-cmac-with-tsip")
                {
                    // check user program key length
                    if (textBoxInitialUserProgramKey_Aes128.Text.Length != 32)
                    {
                        print_log("please specify the correct key size.");
                        return;
                    }
                }
                // check combination of Firmware Verification Type x Use Code signing for AWS IoT

                // check user proguram file path
                if (!File.Exists(textBoxInitialUserProgramFilePath.Text))
                {
                    print_log("please specify the motorola file name.");
                    return;
                }

                // check BootLoader user proguram file path
                if (!File.Exists(textBoxInitialBootLoaderUserProgramFilePath.Text))
                {
                    print_log("please specify the motorola file name.");
                    return;
                }

                if (comboBoxInitialFirmwareVerificationType.Text == "sig-sha256-ecdsa-standalone")
                {
                    // check user private key path
                    if (!File.Exists(textBoxInitialUserPrivateKeyPath.Text))
                    {
                        print_log("please specify the user private key name.");
                        return;
                    }
                }

                if (checkBox1_InitialOutputBinaryFormat.Checked)
                {
                    // Displays a SaveFileDialog so the user can save
                    saveFileDialog.Filter = "Renesas Secure Update|*.rsu";
                    saveFileDialog.Title = "Save an (Encrypted(option)) User Program File";
                    saveFileDialog.FileName = "userprog.rsu";
                }
                else
                {
                    // Displays a SaveFileDialog so the user can save
                    saveFileDialog.Filter = "Motrola S format |*.mot";
                    saveFileDialog.Title = "Save an (Encrypted(option)) User Program File";
                    saveFileDialog.FileName = "userprog.mot";
                }

                if (saveFileDialog.ShowDialog() != DialogResult.OK || saveFileDialog.FileName == "")
                {
                    print_log("please specify the output file name.");
                    return;
                }

                string mcuName = comboBox_Initial_Mcu_firmupdate.Text;
                GenerateInitialUserprog(mcuName);

                print_log("generate succeeded.");
            }
            catch (Exception)
            {
                print_log("exception has occurred.");
            }
        }

        private void groupBox5_Enter(object sender, EventArgs e)
        {

        }

        private void label9_Click(object sender, EventArgs e)
        {

        }

        private void buttonBrowseInitialBootLoaderUserprog_Click(object sender, EventArgs e)
        {
            // Displays a OpenFileDialog so the user can save the Image
            openFileDialog.Filter = "Motorola Format File|*.mot";
            openFileDialog.Title = "Open the Motorola Format File from Boot Loader";
            openFileDialog.FileName = "";

            if (openFileDialog.ShowDialog() != DialogResult.OK || openFileDialog.FileName == "")
            {
                print_log("please specify the motorola file name.");
                return;
            }

            textBoxInitialBootLoaderUserProgramFilePath.Text = openFileDialog.FileName;
        }

        /// <summary>
        /// 指定されたファイルがロックされているかどうかを返します。
        /// </summary>
        /// <param name="path">検証したいファイルへのフルパス</param>
        /// <returns>ロックされているかどうか</returns>
        private bool IsFileLocked(string path)
        {
            FileStream stream = null;

            try
            {
                stream = new FileStream(path, FileMode.Open, FileAccess.ReadWrite, FileShare.None);
            }
            catch
            {
                return true;
            }
            finally
            {
                if (stream != null)
                {
                    stream.Close();
                }
            }

            return false;
        }

        private void checkBox1_InitialOutputBinaryFormat_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox1_InitialOutputBinaryFormat.Checked)
            {
                textBoxInitialBootLoaderUserProgramFilePath.Enabled = false;
            } else
            {
                textBoxInitialBootLoaderUserProgramFilePath.Enabled = true;
            }
        }
    }
}
