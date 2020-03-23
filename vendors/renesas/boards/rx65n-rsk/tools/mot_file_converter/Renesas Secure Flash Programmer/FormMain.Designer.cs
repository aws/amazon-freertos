namespace Renesas_Secure_Flash_Programmer
{
    partial class FormMain
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.linkLabelDLMServer = new System.Windows.Forms.LinkLabel();
            this.label7 = new System.Windows.Forms.Label();
            this.textBoxSessionKey = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.buttonGenerateSessionKey = new System.Windows.Forms.Button();
            this.tabPage4 = new System.Windows.Forms.TabPage();
            this.buttonGenerateKeyFile = new System.Windows.Forms.Button();
            this.groupBox7 = new System.Windows.Forms.GroupBox();
            this.buttonDelete = new System.Windows.Forms.Button();
            this.listViewKeys = new System.Windows.Forms.ListView();
            this.columnHeaderKeyType = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeaderKeyData = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.label26 = new System.Windows.Forms.Label();
            this.buttonRegister = new System.Windows.Forms.Button();
            this.textBoxKeyData = new System.Windows.Forms.TextBox();
            this.comboBoxKeyType = new System.Windows.Forms.ComboBox();
            this.label30 = new System.Windows.Forms.Label();
            this.groupBox8 = new System.Windows.Forms.GroupBox();
            this.label16 = new System.Windows.Forms.Label();
            this.comboBoxEndian = new System.Windows.Forms.ComboBox();
            this.comboBoxMcu_keywrap = new System.Windows.Forms.ComboBox();
            this.label15 = new System.Windows.Forms.Label();
            this.groupBox6 = new System.Windows.Forms.GroupBox();
            this.label14 = new System.Windows.Forms.Label();
            this.buttonBrowseEncryptedSessionKey = new System.Windows.Forms.Button();
            this.buttonBrowseSessionKey = new System.Windows.Forms.Button();
            this.textBoxEncryptedSessionKeyPath = new System.Windows.Forms.TextBox();
            this.textBoxSessionKeyPath = new System.Windows.Forms.TextBox();
            this.textBoxIV = new System.Windows.Forms.TextBox();
            this.label25 = new System.Windows.Forms.Label();
            this.label13 = new System.Windows.Forms.Label();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.textBoxFirmwareSequenceNumber = new System.Windows.Forms.TextBox();
            this.label11 = new System.Windows.Forms.Label();
            this.comboBoxFirmwareVerificationType = new System.Windows.Forms.ComboBox();
            this.label8 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.comboBoxMcu_firmupdate = new System.Windows.Forms.ComboBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.buttonGenerateUserprog = new System.Windows.Forms.Button();
            this.buttonBrowseUserprog = new System.Windows.Forms.Button();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.textBoxUserProgramFilePath = new System.Windows.Forms.TextBox();
            this.textBoxUserProgramKey_Aes128 = new System.Windows.Forms.TextBox();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.buttonBrowseInitialBootLoaderUserprog = new System.Windows.Forms.Button();
            this.label9 = new System.Windows.Forms.Label();
            this.textBoxInitialBootLoaderUserProgramFilePath = new System.Windows.Forms.TextBox();
            this.Generate_Init_Firm = new System.Windows.Forms.Button();
            this.buttonBrowseInitialUserPrivateKey = new System.Windows.Forms.Button();
            this.buttonBrowseInitialUserprog = new System.Windows.Forms.Button();
            this.label21 = new System.Windows.Forms.Label();
            this.label22 = new System.Windows.Forms.Label();
            this.label23 = new System.Windows.Forms.Label();
            this.textBoxInitialUserPrivateKeyPath = new System.Windows.Forms.TextBox();
            this.textBoxInitialUserProgramFilePath = new System.Windows.Forms.TextBox();
            this.textBoxInitialUserProgramKey_Aes128 = new System.Windows.Forms.TextBox();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.textBoxInitialFirmwareSequenceNumber = new System.Windows.Forms.TextBox();
            this.label12 = new System.Windows.Forms.Label();
            this.checkBox1_InitialOutputBinaryFormat = new System.Windows.Forms.CheckBox();
            this.label17 = new System.Windows.Forms.Label();
            this.comboBoxInitialFirmwareVerificationType = new System.Windows.Forms.ComboBox();
            this.label19 = new System.Windows.Forms.Label();
            this.label20 = new System.Windows.Forms.Label();
            this.comboBox_Initial_Mcu_firmupdate = new System.Windows.Forms.ComboBox();
            this.info = new System.Windows.Forms.TextBox();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.tabPage4.SuspendLayout();
            this.groupBox7.SuspendLayout();
            this.groupBox8.SuspendLayout();
            this.groupBox6.SuspendLayout();
            this.tabPage3.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControl1
            // 
            this.tabControl1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage4);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Controls.Add(this.tabPage3);
            this.tabControl1.Location = new System.Drawing.Point(12, 10);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(805, 415);
            this.tabControl1.TabIndex = 0;
            // 
            // tabPage1
            // 
            this.tabPage1.BackColor = System.Drawing.Color.WhiteSmoke;
            this.tabPage1.Controls.Add(this.groupBox1);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(797, 389);
            this.tabPage1.TabIndex = 4;
            this.tabPage1.Text = "session key";
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.linkLabelDLMServer);
            this.groupBox1.Controls.Add(this.label7);
            this.groupBox1.Controls.Add(this.textBoxSessionKey);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.label6);
            this.groupBox1.Controls.Add(this.buttonGenerateSessionKey);
            this.groupBox1.Location = new System.Drawing.Point(6, 6);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(787, 155);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Generate session key";
            // 
            // linkLabelDLMServer
            // 
            this.linkLabelDLMServer.AutoSize = true;
            this.linkLabelDLMServer.Font = new System.Drawing.Font("MS UI Gothic", 9.75F, System.Drawing.FontStyle.Bold);
            this.linkLabelDLMServer.Location = new System.Drawing.Point(329, 102);
            this.linkLabelDLMServer.Name = "linkLabelDLMServer";
            this.linkLabelDLMServer.Size = new System.Drawing.Size(177, 13);
            this.linkLabelDLMServer.TabIndex = 8;
            this.linkLabelDLMServer.TabStop = true;
            this.linkLabelDLMServer.Text = "https://dlm.renesas.com/";
            this.linkLabelDLMServer.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkLabelDLMServer_LinkClicked);
            // 
            // label7
            // 
            this.label7.Font = new System.Drawing.Font("MS UI Gothic", 9.75F, System.Drawing.FontStyle.Bold);
            this.label7.Location = new System.Drawing.Point(180, 102);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(143, 23);
            this.label7.TabIndex = 7;
            this.label7.Text = "Renesas DLM server";
            // 
            // textBoxSessionKey
            // 
            this.textBoxSessionKey.Location = new System.Drawing.Point(183, 26);
            this.textBoxSessionKey.MaxLength = 64;
            this.textBoxSessionKey.Name = "textBoxSessionKey";
            this.textBoxSessionKey.Size = new System.Drawing.Size(409, 19);
            this.textBoxSessionKey.TabIndex = 1;
            this.textBoxSessionKey.Text = "(Random)";
            // 
            // label1
            // 
            this.label1.Font = new System.Drawing.Font("MS UI Gothic", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
            this.label1.Location = new System.Drawing.Point(181, 67);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(417, 35);
            this.label1.TabIndex = 5;
            this.label1.Text = "Send PGP encrypted session key file to the follow Web Site. \r\nRefer to Renesas DL" +
    "M server FAQ for details.";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(6, 50);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(0, 12);
            this.label2.TabIndex = 2;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(6, 29);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(157, 24);
            this.label6.TabIndex = 0;
            this.label6.Text = "session key Value \r\n(32 byte hex / 64 characters)";
            // 
            // buttonGenerateSessionKey
            // 
            this.buttonGenerateSessionKey.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonGenerateSessionKey.Location = new System.Drawing.Point(621, 126);
            this.buttonGenerateSessionKey.Name = "buttonGenerateSessionKey";
            this.buttonGenerateSessionKey.Size = new System.Drawing.Size(159, 23);
            this.buttonGenerateSessionKey.TabIndex = 6;
            this.buttonGenerateSessionKey.Text = "format to DLM server file...";
            this.buttonGenerateSessionKey.UseVisualStyleBackColor = true;
            this.buttonGenerateSessionKey.Click += new System.EventHandler(this.buttonGenerateSessionKey_Click);
            // 
            // tabPage4
            // 
            this.tabPage4.BackColor = System.Drawing.Color.WhiteSmoke;
            this.tabPage4.Controls.Add(this.buttonGenerateKeyFile);
            this.tabPage4.Controls.Add(this.groupBox7);
            this.tabPage4.Controls.Add(this.groupBox8);
            this.tabPage4.Controls.Add(this.groupBox6);
            this.tabPage4.Location = new System.Drawing.Point(4, 22);
            this.tabPage4.Name = "tabPage4";
            this.tabPage4.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage4.Size = new System.Drawing.Size(797, 389);
            this.tabPage4.TabIndex = 3;
            this.tabPage4.Text = "Key Wrap";
            // 
            // buttonGenerateKeyFile
            // 
            this.buttonGenerateKeyFile.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonGenerateKeyFile.Location = new System.Drawing.Point(657, 361);
            this.buttonGenerateKeyFile.Name = "buttonGenerateKeyFile";
            this.buttonGenerateKeyFile.Size = new System.Drawing.Size(134, 23);
            this.buttonGenerateKeyFile.TabIndex = 3;
            this.buttonGenerateKeyFile.Text = "Generate Key Files...";
            this.buttonGenerateKeyFile.UseVisualStyleBackColor = true;
            this.buttonGenerateKeyFile.Click += new System.EventHandler(this.buttonGenerateKeyFile_Click);
            // 
            // groupBox7
            // 
            this.groupBox7.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox7.Controls.Add(this.buttonDelete);
            this.groupBox7.Controls.Add(this.listViewKeys);
            this.groupBox7.Controls.Add(this.label26);
            this.groupBox7.Controls.Add(this.buttonRegister);
            this.groupBox7.Controls.Add(this.textBoxKeyData);
            this.groupBox7.Controls.Add(this.comboBoxKeyType);
            this.groupBox7.Controls.Add(this.label30);
            this.groupBox7.Location = new System.Drawing.Point(6, 70);
            this.groupBox7.Name = "groupBox7";
            this.groupBox7.Size = new System.Drawing.Size(787, 157);
            this.groupBox7.TabIndex = 1;
            this.groupBox7.TabStop = false;
            this.groupBox7.Text = "Key Setting";
            // 
            // buttonDelete
            // 
            this.buttonDelete.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonDelete.Location = new System.Drawing.Point(706, 61);
            this.buttonDelete.Name = "buttonDelete";
            this.buttonDelete.Size = new System.Drawing.Size(75, 23);
            this.buttonDelete.TabIndex = 6;
            this.buttonDelete.Text = "Delete";
            this.buttonDelete.UseVisualStyleBackColor = true;
            this.buttonDelete.Click += new System.EventHandler(this.buttonDelete_Click);
            // 
            // listViewKeys
            // 
            this.listViewKeys.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.listViewKeys.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeaderKeyType,
            this.columnHeaderKeyData});
            this.listViewKeys.FullRowSelect = true;
            this.listViewKeys.Location = new System.Drawing.Point(11, 61);
            this.listViewKeys.MultiSelect = false;
            this.listViewKeys.Name = "listViewKeys";
            this.listViewKeys.Size = new System.Drawing.Size(690, 81);
            this.listViewKeys.TabIndex = 4;
            this.listViewKeys.UseCompatibleStateImageBehavior = false;
            this.listViewKeys.View = System.Windows.Forms.View.Details;
            // 
            // columnHeaderKeyType
            // 
            this.columnHeaderKeyType.Text = "Key Type";
            this.columnHeaderKeyType.Width = 150;
            // 
            // columnHeaderKeyData
            // 
            this.columnHeaderKeyData.Text = "Key Data";
            this.columnHeaderKeyData.Width = 580;
            // 
            // label26
            // 
            this.label26.AutoSize = true;
            this.label26.Location = new System.Drawing.Point(219, 27);
            this.label26.Name = "label26";
            this.label26.Size = new System.Drawing.Size(52, 12);
            this.label26.TabIndex = 2;
            this.label26.Text = "Key Data";
            // 
            // buttonRegister
            // 
            this.buttonRegister.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonRegister.Location = new System.Drawing.Point(706, 23);
            this.buttonRegister.Name = "buttonRegister";
            this.buttonRegister.Size = new System.Drawing.Size(75, 23);
            this.buttonRegister.TabIndex = 5;
            this.buttonRegister.Text = "Register";
            this.buttonRegister.UseVisualStyleBackColor = true;
            this.buttonRegister.Click += new System.EventHandler(this.buttonRegister_Click);
            // 
            // textBoxKeyData
            // 
            this.textBoxKeyData.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxKeyData.Location = new System.Drawing.Point(274, 25);
            this.textBoxKeyData.MaxLength = 1024;
            this.textBoxKeyData.Name = "textBoxKeyData";
            this.textBoxKeyData.Size = new System.Drawing.Size(427, 19);
            this.textBoxKeyData.TabIndex = 3;
            // 
            // comboBoxKeyType
            // 
            this.comboBoxKeyType.FormattingEnabled = true;
            this.comboBoxKeyType.Location = new System.Drawing.Point(65, 25);
            this.comboBoxKeyType.Name = "comboBoxKeyType";
            this.comboBoxKeyType.Size = new System.Drawing.Size(136, 20);
            this.comboBoxKeyType.TabIndex = 1;
            // 
            // label30
            // 
            this.label30.AutoSize = true;
            this.label30.Location = new System.Drawing.Point(6, 27);
            this.label30.Name = "label30";
            this.label30.Size = new System.Drawing.Size(53, 12);
            this.label30.TabIndex = 0;
            this.label30.Text = "Key Type";
            // 
            // groupBox8
            // 
            this.groupBox8.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox8.Controls.Add(this.label16);
            this.groupBox8.Controls.Add(this.comboBoxEndian);
            this.groupBox8.Controls.Add(this.comboBoxMcu_keywrap);
            this.groupBox8.Controls.Add(this.label15);
            this.groupBox8.Location = new System.Drawing.Point(6, 6);
            this.groupBox8.Name = "groupBox8";
            this.groupBox8.Size = new System.Drawing.Size(787, 57);
            this.groupBox8.TabIndex = 0;
            this.groupBox8.TabStop = false;
            this.groupBox8.Text = "MCU";
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point(326, 25);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(39, 12);
            this.label16.TabIndex = 2;
            this.label16.Text = "Endian";
            // 
            // comboBoxEndian
            // 
            this.comboBoxEndian.FormattingEnabled = true;
            this.comboBoxEndian.Location = new System.Drawing.Point(381, 22);
            this.comboBoxEndian.Name = "comboBoxEndian";
            this.comboBoxEndian.Size = new System.Drawing.Size(180, 20);
            this.comboBoxEndian.TabIndex = 3;
            // 
            // comboBoxMcu_keywrap
            // 
            this.comboBoxMcu_keywrap.FormattingEnabled = true;
            this.comboBoxMcu_keywrap.Location = new System.Drawing.Point(103, 22);
            this.comboBoxMcu_keywrap.Name = "comboBoxMcu_keywrap";
            this.comboBoxMcu_keywrap.Size = new System.Drawing.Size(180, 20);
            this.comboBoxMcu_keywrap.TabIndex = 1;
            this.comboBoxMcu_keywrap.Text = "(select MCU)";
            this.comboBoxMcu_keywrap.SelectedIndexChanged += new System.EventHandler(this.comboBoxMcu_keywrap_SelectedIndexChanged);
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(6, 25);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(66, 12);
            this.label15.TabIndex = 0;
            this.label15.Text = "Select MCU";
            // 
            // groupBox6
            // 
            this.groupBox6.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox6.Controls.Add(this.label14);
            this.groupBox6.Controls.Add(this.buttonBrowseEncryptedSessionKey);
            this.groupBox6.Controls.Add(this.buttonBrowseSessionKey);
            this.groupBox6.Controls.Add(this.textBoxEncryptedSessionKeyPath);
            this.groupBox6.Controls.Add(this.textBoxSessionKeyPath);
            this.groupBox6.Controls.Add(this.textBoxIV);
            this.groupBox6.Controls.Add(this.label25);
            this.groupBox6.Controls.Add(this.label13);
            this.groupBox6.Location = new System.Drawing.Point(7, 233);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Size = new System.Drawing.Size(787, 115);
            this.groupBox6.TabIndex = 2;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "session key";
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(6, 25);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(116, 12);
            this.label14.TabIndex = 0;
            this.label14.Text = "session key File Path";
            // 
            // buttonBrowseEncryptedSessionKey
            // 
            this.buttonBrowseEncryptedSessionKey.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonBrowseEncryptedSessionKey.Location = new System.Drawing.Point(548, 45);
            this.buttonBrowseEncryptedSessionKey.Name = "buttonBrowseEncryptedSessionKey";
            this.buttonBrowseEncryptedSessionKey.Size = new System.Drawing.Size(75, 23);
            this.buttonBrowseEncryptedSessionKey.TabIndex = 5;
            this.buttonBrowseEncryptedSessionKey.Text = "Browse...";
            this.buttonBrowseEncryptedSessionKey.UseVisualStyleBackColor = true;
            this.buttonBrowseEncryptedSessionKey.Click += new System.EventHandler(this.buttonBrowseEncryptedSessionKey_Click);
            // 
            // buttonBrowseSessionKey
            // 
            this.buttonBrowseSessionKey.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonBrowseSessionKey.Location = new System.Drawing.Point(548, 20);
            this.buttonBrowseSessionKey.Name = "buttonBrowseSessionKey";
            this.buttonBrowseSessionKey.Size = new System.Drawing.Size(75, 23);
            this.buttonBrowseSessionKey.TabIndex = 2;
            this.buttonBrowseSessionKey.Text = "Browse...";
            this.buttonBrowseSessionKey.UseVisualStyleBackColor = true;
            this.buttonBrowseSessionKey.Click += new System.EventHandler(this.buttonBrowseSessionKey_Click);
            // 
            // textBoxEncryptedSessionKeyPath
            // 
            this.textBoxEncryptedSessionKeyPath.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxEncryptedSessionKeyPath.Location = new System.Drawing.Point(183, 47);
            this.textBoxEncryptedSessionKeyPath.Name = "textBoxEncryptedSessionKeyPath";
            this.textBoxEncryptedSessionKeyPath.Size = new System.Drawing.Size(359, 19);
            this.textBoxEncryptedSessionKeyPath.TabIndex = 4;
            // 
            // textBoxSessionKeyPath
            // 
            this.textBoxSessionKeyPath.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxSessionKeyPath.Location = new System.Drawing.Point(183, 22);
            this.textBoxSessionKeyPath.Name = "textBoxSessionKeyPath";
            this.textBoxSessionKeyPath.Size = new System.Drawing.Size(359, 19);
            this.textBoxSessionKeyPath.TabIndex = 1;
            // 
            // textBoxIV
            // 
            this.textBoxIV.Location = new System.Drawing.Point(183, 79);
            this.textBoxIV.MaxLength = 32;
            this.textBoxIV.Name = "textBoxIV";
            this.textBoxIV.Size = new System.Drawing.Size(359, 19);
            this.textBoxIV.TabIndex = 7;
            this.textBoxIV.Text = "(Random)";
            // 
            // label25
            // 
            this.label25.AutoSize = true;
            this.label25.Location = new System.Drawing.Point(6, 82);
            this.label25.Name = "label25";
            this.label25.Size = new System.Drawing.Size(172, 12);
            this.label25.TabIndex = 6;
            this.label25.Text = "IV (16 byte hex / 32 characters)";
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(6, 50);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(170, 12);
            this.label13.TabIndex = 3;
            this.label13.Text = "encrypted session key File Path";
            // 
            // tabPage3
            // 
            this.tabPage3.BackColor = System.Drawing.Color.WhiteSmoke;
            this.tabPage3.Controls.Add(this.groupBox3);
            this.tabPage3.Controls.Add(this.groupBox2);
            this.tabPage3.Location = new System.Drawing.Point(4, 22);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage3.Size = new System.Drawing.Size(797, 389);
            this.tabPage3.TabIndex = 2;
            this.tabPage3.Text = "Update Firm";
            // 
            // groupBox3
            // 
            this.groupBox3.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox3.Controls.Add(this.textBoxFirmwareSequenceNumber);
            this.groupBox3.Controls.Add(this.label11);
            this.groupBox3.Controls.Add(this.comboBoxFirmwareVerificationType);
            this.groupBox3.Controls.Add(this.label8);
            this.groupBox3.Controls.Add(this.label5);
            this.groupBox3.Controls.Add(this.comboBoxMcu_firmupdate);
            this.groupBox3.Location = new System.Drawing.Point(6, 6);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(787, 153);
            this.groupBox3.TabIndex = 0;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Settings";
            // 
            // textBoxFirmwareSequenceNumber
            // 
            this.textBoxFirmwareSequenceNumber.Location = new System.Drawing.Point(224, 109);
            this.textBoxFirmwareSequenceNumber.Name = "textBoxFirmwareSequenceNumber";
            this.textBoxFirmwareSequenceNumber.Size = new System.Drawing.Size(156, 19);
            this.textBoxFirmwareSequenceNumber.TabIndex = 9;
            this.textBoxFirmwareSequenceNumber.Text = "(1 - 4294967295)";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(6, 112);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(148, 12);
            this.label11.TabIndex = 8;
            this.label11.Text = "Firmware Sequence Number";
            // 
            // comboBoxFirmwareVerificationType
            // 
            this.comboBoxFirmwareVerificationType.FormattingEnabled = true;
            this.comboBoxFirmwareVerificationType.Location = new System.Drawing.Point(224, 64);
            this.comboBoxFirmwareVerificationType.Name = "comboBoxFirmwareVerificationType";
            this.comboBoxFirmwareVerificationType.Size = new System.Drawing.Size(303, 20);
            this.comboBoxFirmwareVerificationType.TabIndex = 3;
            this.comboBoxFirmwareVerificationType.Text = "(select Firmware Verification Type)";
            this.comboBoxFirmwareVerificationType.SelectedIndexChanged += new System.EventHandler(this.comboBoxFirmwareVerificationType_SelectedIndexChanged_1);
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(6, 67);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(180, 12);
            this.label8.TabIndex = 2;
            this.label8.Text = "Select Firmware Verification Type";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(6, 25);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(66, 12);
            this.label5.TabIndex = 0;
            this.label5.Text = "Select MCU";
            // 
            // comboBoxMcu_firmupdate
            // 
            this.comboBoxMcu_firmupdate.FormattingEnabled = true;
            this.comboBoxMcu_firmupdate.Location = new System.Drawing.Point(224, 22);
            this.comboBoxMcu_firmupdate.Name = "comboBoxMcu_firmupdate";
            this.comboBoxMcu_firmupdate.Size = new System.Drawing.Size(303, 20);
            this.comboBoxMcu_firmupdate.TabIndex = 1;
            this.comboBoxMcu_firmupdate.Text = "(select MCU)";
            // 
            // groupBox2
            // 
            this.groupBox2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox2.Controls.Add(this.buttonGenerateUserprog);
            this.groupBox2.Controls.Add(this.buttonBrowseUserprog);
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Controls.Add(this.label3);
            this.groupBox2.Controls.Add(this.textBoxUserProgramFilePath);
            this.groupBox2.Controls.Add(this.textBoxUserProgramKey_Aes128);
            this.groupBox2.Location = new System.Drawing.Point(6, 217);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(787, 166);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "User Program";
            // 
            // buttonGenerateUserprog
            // 
            this.buttonGenerateUserprog.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonGenerateUserprog.Location = new System.Drawing.Point(668, 136);
            this.buttonGenerateUserprog.Name = "buttonGenerateUserprog";
            this.buttonGenerateUserprog.Size = new System.Drawing.Size(113, 23);
            this.buttonGenerateUserprog.TabIndex = 5;
            this.buttonGenerateUserprog.Text = "Generate...";
            this.buttonGenerateUserprog.UseVisualStyleBackColor = true;
            this.buttonGenerateUserprog.Click += new System.EventHandler(this.buttonGenerateUserprog_Click);
            // 
            // buttonBrowseUserprog
            // 
            this.buttonBrowseUserprog.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonBrowseUserprog.Location = new System.Drawing.Point(589, 107);
            this.buttonBrowseUserprog.Name = "buttonBrowseUserprog";
            this.buttonBrowseUserprog.Size = new System.Drawing.Size(75, 23);
            this.buttonBrowseUserprog.TabIndex = 4;
            this.buttonBrowseUserprog.Text = "Browse...";
            this.buttonBrowseUserprog.UseVisualStyleBackColor = true;
            this.buttonBrowseUserprog.Click += new System.EventHandler(this.buttonBrowseUserprog_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(6, 112);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(141, 12);
            this.label4.TabIndex = 2;
            this.label4.Text = "File Path (Motrola Format)";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(6, 25);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(157, 24);
            this.label3.TabIndex = 0;
            this.label3.Text = "AES MAC Key \r\n(16 byte hex / 32 characters)";
            // 
            // textBoxUserProgramFilePath
            // 
            this.textBoxUserProgramFilePath.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxUserProgramFilePath.Location = new System.Drawing.Point(224, 109);
            this.textBoxUserProgramFilePath.Name = "textBoxUserProgramFilePath";
            this.textBoxUserProgramFilePath.Size = new System.Drawing.Size(359, 19);
            this.textBoxUserProgramFilePath.TabIndex = 3;
            // 
            // textBoxUserProgramKey_Aes128
            // 
            this.textBoxUserProgramKey_Aes128.Location = new System.Drawing.Point(224, 30);
            this.textBoxUserProgramKey_Aes128.MaxLength = 32;
            this.textBoxUserProgramKey_Aes128.Name = "textBoxUserProgramKey_Aes128";
            this.textBoxUserProgramKey_Aes128.Size = new System.Drawing.Size(359, 19);
            this.textBoxUserProgramKey_Aes128.TabIndex = 1;
            this.textBoxUserProgramKey_Aes128.Enabled = false;
            // 
            // tabPage2
            // 
            this.tabPage2.BackColor = System.Drawing.Color.WhiteSmoke;
            this.tabPage2.Controls.Add(this.groupBox5);
            this.tabPage2.Controls.Add(this.groupBox4);
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(797, 389);
            this.tabPage2.TabIndex = 5;
            this.tabPage2.Text = "Initial Firm";
            this.tabPage2.Click += new System.EventHandler(this.tabPage2_Click);
            // 
            // groupBox5
            // 
            this.groupBox5.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox5.Controls.Add(this.buttonBrowseInitialBootLoaderUserprog);
            this.groupBox5.Controls.Add(this.label9);
            this.groupBox5.Controls.Add(this.textBoxInitialBootLoaderUserProgramFilePath);
            this.groupBox5.Controls.Add(this.Generate_Init_Firm);
            this.groupBox5.Controls.Add(this.buttonBrowseInitialUserPrivateKey);
            this.groupBox5.Controls.Add(this.buttonBrowseInitialUserprog);
            this.groupBox5.Controls.Add(this.label21);
            this.groupBox5.Controls.Add(this.label22);
            this.groupBox5.Controls.Add(this.label23);
            this.groupBox5.Controls.Add(this.textBoxInitialUserPrivateKeyPath);
            this.groupBox5.Controls.Add(this.textBoxInitialUserProgramFilePath);
            this.groupBox5.Controls.Add(this.textBoxInitialUserProgramKey_Aes128);
            this.groupBox5.Location = new System.Drawing.Point(4, 207);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(787, 176);
            this.groupBox5.TabIndex = 2;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "User Program";
            this.groupBox5.Enter += new System.EventHandler(this.groupBox5_Enter);
            // 
            // buttonBrowseInitialBootLoaderUserprog
            // 
            this.buttonBrowseInitialBootLoaderUserprog.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonBrowseInitialBootLoaderUserprog.Location = new System.Drawing.Point(589, 108);
            this.buttonBrowseInitialBootLoaderUserprog.Name = "buttonBrowseInitialBootLoaderUserprog";
            this.buttonBrowseInitialBootLoaderUserprog.Size = new System.Drawing.Size(75, 23);
            this.buttonBrowseInitialBootLoaderUserprog.TabIndex = 8;
            this.buttonBrowseInitialBootLoaderUserprog.Text = "Browse...";
            this.buttonBrowseInitialBootLoaderUserprog.UseVisualStyleBackColor = true;
            this.buttonBrowseInitialBootLoaderUserprog.Click += new System.EventHandler(this.buttonBrowseInitialBootLoaderUserprog_Click);
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(6, 113);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(207, 12);
            this.label9.TabIndex = 6;
            this.label9.Text = "Boot Loader File Path (Motrola Format)";
            this.label9.Click += new System.EventHandler(this.label9_Click);
            // 
            // textBoxInitialBootLoaderUserProgramFilePath
            // 
            this.textBoxInitialBootLoaderUserProgramFilePath.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxInitialBootLoaderUserProgramFilePath.Enabled = false;
            this.textBoxInitialBootLoaderUserProgramFilePath.Location = new System.Drawing.Point(224, 110);
            this.textBoxInitialBootLoaderUserProgramFilePath.Name = "textBoxInitialBootLoaderUserProgramFilePath";
            this.textBoxInitialBootLoaderUserProgramFilePath.Size = new System.Drawing.Size(359, 19);
            this.textBoxInitialBootLoaderUserProgramFilePath.TabIndex = 7;
            // 
            // Generate_Init_Firm
            // 
            this.Generate_Init_Firm.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.Generate_Init_Firm.Location = new System.Drawing.Point(688, 146);
            this.Generate_Init_Firm.Name = "Generate_Init_Firm";
            this.Generate_Init_Firm.Size = new System.Drawing.Size(93, 23);
            this.Generate_Init_Firm.TabIndex = 5;
            this.Generate_Init_Firm.Text = "Generate...";
            this.Generate_Init_Firm.UseVisualStyleBackColor = true;
            this.Generate_Init_Firm.Click += new System.EventHandler(this.buttonGenerateInitialUserprog);
            // 
            // buttonBrowseInitialUserPrivateKey
            // 
            this.buttonBrowseInitialUserPrivateKey.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonBrowseInitialUserPrivateKey.Location = new System.Drawing.Point(589, 69);
            this.buttonBrowseInitialUserPrivateKey.Name = "buttonBrowseInitialUserPrivateKey";
            this.buttonBrowseInitialUserPrivateKey.Size = new System.Drawing.Size(75, 23);
            this.buttonBrowseInitialUserPrivateKey.TabIndex = 4;
            this.buttonBrowseInitialUserPrivateKey.Text = "Browse...";
            this.buttonBrowseInitialUserPrivateKey.UseVisualStyleBackColor = true;
            this.buttonBrowseInitialUserPrivateKey.Click += new System.EventHandler(this.buttonBrowseInitialUserPrivateKey_Click);
            // 
            // buttonBrowseInitialUserprog
            // 
            this.buttonBrowseInitialUserprog.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonBrowseInitialUserprog.Location = new System.Drawing.Point(589, 146);
            this.buttonBrowseInitialUserprog.Name = "buttonBrowseInitialUserprog";
            this.buttonBrowseInitialUserprog.Size = new System.Drawing.Size(75, 23);
            this.buttonBrowseInitialUserprog.TabIndex = 4;
            this.buttonBrowseInitialUserprog.Text = "Browse...";
            this.buttonBrowseInitialUserprog.UseVisualStyleBackColor = true;
            this.buttonBrowseInitialUserprog.Click += new System.EventHandler(this.buttonBrowseInitialUserprog_Click);
            // 
            // label21
            // 
            this.label21.AutoSize = true;
            this.label21.Location = new System.Drawing.Point(6, 74);
            this.label21.Name = "label21";
            this.label21.Size = new System.Drawing.Size(163, 12);
            this.label21.TabIndex = 2;
            this.label21.Text = "Private Key Path (PEM format)";
            // 
            // label22
            // 
            this.label22.AutoSize = true;
            this.label22.Location = new System.Drawing.Point(6, 151);
            this.label22.Name = "label22";
            this.label22.Size = new System.Drawing.Size(141, 12);
            this.label22.TabIndex = 2;
            this.label22.Text = "File Path (Motrola Format)";
            // 
            // label23
            // 
            this.label23.AutoSize = true;
            this.label23.Location = new System.Drawing.Point(6, 25);
            this.label23.Name = "label23";
            this.label23.Size = new System.Drawing.Size(157, 24);
            this.label23.TabIndex = 0;
            this.label23.Text = "AES MAC Key \r\n(16 byte hex / 32 characters)";
            // 
            // textBoxInitialUserPrivateKeyPath
            // 
            this.textBoxInitialUserPrivateKeyPath.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxInitialUserPrivateKeyPath.Location = new System.Drawing.Point(224, 71);
            this.textBoxInitialUserPrivateKeyPath.Name = "textBoxInitialUserPrivateKeyPath";
            this.textBoxInitialUserPrivateKeyPath.Size = new System.Drawing.Size(359, 19);
            this.textBoxInitialUserPrivateKeyPath.TabIndex = 3;
            // 
            // textBoxInitialUserProgramFilePath
            // 
            this.textBoxInitialUserProgramFilePath.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxInitialUserProgramFilePath.Location = new System.Drawing.Point(224, 148);
            this.textBoxInitialUserProgramFilePath.Name = "textBoxInitialUserProgramFilePath";
            this.textBoxInitialUserProgramFilePath.Size = new System.Drawing.Size(359, 19);
            this.textBoxInitialUserProgramFilePath.TabIndex = 3;
            // 
            // textBoxInitialUserProgramKey_Aes128
            // 
            this.textBoxInitialUserProgramKey_Aes128.Location = new System.Drawing.Point(224, 30);
            this.textBoxInitialUserProgramKey_Aes128.MaxLength = 32;
            this.textBoxInitialUserProgramKey_Aes128.Name = "textBoxInitialUserProgramKey_Aes128";
            this.textBoxInitialUserProgramKey_Aes128.Size = new System.Drawing.Size(359, 19);
            this.textBoxInitialUserProgramKey_Aes128.TabIndex = 1;
            this.textBoxInitialUserProgramKey_Aes128.Enabled = false;
            // 
            // groupBox4
            // 
            this.groupBox4.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox4.BackColor = System.Drawing.Color.WhiteSmoke;
            this.groupBox4.Controls.Add(this.textBoxInitialFirmwareSequenceNumber);
            this.groupBox4.Controls.Add(this.label12);
            this.groupBox4.Controls.Add(this.checkBox1_InitialOutputBinaryFormat);
            this.groupBox4.Controls.Add(this.label17);
            this.groupBox4.Controls.Add(this.comboBoxInitialFirmwareVerificationType);
            this.groupBox4.Controls.Add(this.label19);
            this.groupBox4.Controls.Add(this.label20);
            this.groupBox4.Controls.Add(this.comboBox_Initial_Mcu_firmupdate);
            this.groupBox4.Location = new System.Drawing.Point(3, 6);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(787, 184);
            this.groupBox4.TabIndex = 1;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Settings";
            // 
            // textBoxInitialFirmwareSequenceNumber
            // 
            this.textBoxInitialFirmwareSequenceNumber.Location = new System.Drawing.Point(224, 109);
            this.textBoxInitialFirmwareSequenceNumber.Name = "textBoxInitialFirmwareSequenceNumber";
            this.textBoxInitialFirmwareSequenceNumber.Size = new System.Drawing.Size(156, 19);
            this.textBoxInitialFirmwareSequenceNumber.TabIndex = 9;
            this.textBoxInitialFirmwareSequenceNumber.Text = "(1 - 4294967295)";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(6, 112);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(148, 12);
            this.label12.TabIndex = 8;
            this.label12.Text = "Firmware Sequence Number";
            // 
            // checkBox1_InitialOutputBinaryFormat
            // 
            this.checkBox1_InitialOutputBinaryFormat.AutoSize = true;
            this.checkBox1_InitialOutputBinaryFormat.Checked = true;
            this.checkBox1_InitialOutputBinaryFormat.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBox1_InitialOutputBinaryFormat.Location = new System.Drawing.Point(224, 153);
            this.checkBox1_InitialOutputBinaryFormat.Name = "checkBox1_InitialOutputBinaryFormat";
            this.checkBox1_InitialOutputBinaryFormat.Size = new System.Drawing.Size(15, 14);
            this.checkBox1_InitialOutputBinaryFormat.TabIndex = 7;
            this.checkBox1_InitialOutputBinaryFormat.UseVisualStyleBackColor = true;
            this.checkBox1_InitialOutputBinaryFormat.CheckedChanged += new System.EventHandler(this.checkBox1_InitialOutputBinaryFormat_CheckedChanged);
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Location = new System.Drawing.Point(6, 153);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(116, 12);
            this.label17.TabIndex = 6;
            this.label17.Text = "Output Binary Format";
            // 
            // comboBoxInitialFirmwareVerificationType
            // 
            this.comboBoxInitialFirmwareVerificationType.FormattingEnabled = true;
            this.comboBoxInitialFirmwareVerificationType.Location = new System.Drawing.Point(224, 64);
            this.comboBoxInitialFirmwareVerificationType.Name = "comboBoxInitialFirmwareVerificationType";
            this.comboBoxInitialFirmwareVerificationType.Size = new System.Drawing.Size(303, 20);
            this.comboBoxInitialFirmwareVerificationType.TabIndex = 3;
            this.comboBoxInitialFirmwareVerificationType.Text = "(select Firmware Verification Type)";
            this.comboBoxInitialFirmwareVerificationType.SelectedIndexChanged += new System.EventHandler(this.comboBoxInitialFirmwareVerificationType_SelectedIndexChanged);
            // 
            // label19
            // 
            this.label19.AutoSize = true;
            this.label19.Location = new System.Drawing.Point(6, 67);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(180, 12);
            this.label19.TabIndex = 2;
            this.label19.Text = "Select Firmware Verification Type";
            // 
            // label20
            // 
            this.label20.AutoSize = true;
            this.label20.Location = new System.Drawing.Point(6, 25);
            this.label20.Name = "label20";
            this.label20.Size = new System.Drawing.Size(66, 12);
            this.label20.TabIndex = 0;
            this.label20.Text = "Select MCU";
            // 
            // comboBox_Initial_Mcu_firmupdate
            // 
            this.comboBox_Initial_Mcu_firmupdate.FormattingEnabled = true;
            this.comboBox_Initial_Mcu_firmupdate.Location = new System.Drawing.Point(224, 22);
            this.comboBox_Initial_Mcu_firmupdate.Name = "comboBox_Initial_Mcu_firmupdate";
            this.comboBox_Initial_Mcu_firmupdate.Size = new System.Drawing.Size(303, 20);
            this.comboBox_Initial_Mcu_firmupdate.TabIndex = 1;
            this.comboBox_Initial_Mcu_firmupdate.Text = "(select MCU)";
            this.comboBox_Initial_Mcu_firmupdate.SelectedIndexChanged += new System.EventHandler(this.comboBox2_SelectedIndexChanged);
            // 
            // info
            // 
            this.info.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.info.BackColor = System.Drawing.SystemColors.Info;
            this.info.Location = new System.Drawing.Point(12, 439);
            this.info.Multiline = true;
            this.info.Name = "info";
            this.info.Size = new System.Drawing.Size(806, 74);
            this.info.TabIndex = 1;
            // 
            // FormMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(829, 528);
            this.Controls.Add(this.info);
            this.Controls.Add(this.tabControl1);
            this.MaximizeBox = false;
            this.Name = "FormMain";
            this.Text = "Renesas Secure Flash Programmer";
            this.Load += new System.EventHandler(this.FormMain_Load);
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.tabPage4.ResumeLayout(false);
            this.groupBox7.ResumeLayout(false);
            this.groupBox7.PerformLayout();
            this.groupBox8.ResumeLayout(false);
            this.groupBox8.PerformLayout();
            this.groupBox6.ResumeLayout(false);
            this.groupBox6.PerformLayout();
            this.tabPage3.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.tabPage2.ResumeLayout(false);
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.TabPage tabPage3;
        private System.Windows.Forms.TextBox info;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button buttonGenerateUserprog;
		private System.Windows.Forms.Button buttonBrowseUserprog;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.TextBox textBoxUserProgramFilePath;
		private System.Windows.Forms.TabPage tabPage4;
        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.ComboBox comboBoxEndian;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.GroupBox groupBox8;
        private System.Windows.Forms.ComboBox comboBoxMcu_keywrap;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.TextBox textBoxIV;
        private System.Windows.Forms.Label label25;
        private System.Windows.Forms.GroupBox groupBox7;
        private System.Windows.Forms.ComboBox comboBoxKeyType;
        private System.Windows.Forms.Label label30;
        private System.Windows.Forms.ListView listViewKeys;
        private System.Windows.Forms.Label label26;
        private System.Windows.Forms.Button buttonRegister;
        private System.Windows.Forms.TextBox textBoxKeyData;
        private System.Windows.Forms.Button buttonBrowseEncryptedSessionKey;
        private System.Windows.Forms.TextBox textBoxEncryptedSessionKeyPath;
        private System.Windows.Forms.Button buttonBrowseSessionKey;
        private System.Windows.Forms.TextBox textBoxSessionKeyPath;
        private System.Windows.Forms.Button buttonGenerateKeyFile;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.ToolTip toolTip1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox textBoxSessionKey;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Button buttonGenerateSessionKey;
        private System.Windows.Forms.ColumnHeader columnHeaderKeyType;
        private System.Windows.Forms.ColumnHeader columnHeaderKeyData;
        private System.Windows.Forms.Button buttonDelete;
        private System.Windows.Forms.LinkLabel linkLabelDLMServer;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox textBoxUserProgramKey_Aes128;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox comboBoxMcu_firmupdate;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.ComboBox comboBoxFirmwareVerificationType;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.TextBox textBoxFirmwareSequenceNumber;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.TextBox textBoxInitialFirmwareSequenceNumber;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.CheckBox checkBox1_InitialOutputBinaryFormat;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.ComboBox comboBoxInitialFirmwareVerificationType;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.Label label20;
        private System.Windows.Forms.ComboBox comboBox_Initial_Mcu_firmupdate;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.Button Generate_Init_Firm;
        private System.Windows.Forms.Button buttonBrowseInitialUserPrivateKey;
        private System.Windows.Forms.Button buttonBrowseInitialUserprog;
        private System.Windows.Forms.Label label21;
        private System.Windows.Forms.Label label22;
        private System.Windows.Forms.Label label23;
        private System.Windows.Forms.TextBox textBoxInitialUserPrivateKeyPath;
        private System.Windows.Forms.TextBox textBoxInitialUserProgramFilePath;
        private System.Windows.Forms.TextBox textBoxInitialUserProgramKey_Aes128;
        private System.Windows.Forms.Button buttonBrowseInitialBootLoaderUserprog;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.TextBox textBoxInitialBootLoaderUserProgramFilePath;
    }
}