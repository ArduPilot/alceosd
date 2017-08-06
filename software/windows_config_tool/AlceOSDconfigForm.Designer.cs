namespace AlceOSD_updater
{
    partial class AlceOSDconfigForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AlceOSDconfigForm));
            this.bt_dnCfg = new System.Windows.Forms.Button();
            this.bt_upCfg = new System.Windows.Forms.Button();
            this.tb_tlog = new System.Windows.Forms.TrackBar();
            this.bt_sendTlog = new System.Windows.Forms.Button();
            this.bt_reboot = new System.Windows.Forms.Button();
            this.cbx_mavmode = new System.Windows.Forms.CheckBox();
            this.label60 = new System.Windows.Forms.Label();
            this.bt_submitCfg = new System.Windows.Forms.Button();
            this.lb_history = new System.Windows.Forms.ListBox();
            this.tb_cmdLine = new System.Windows.Forms.TextBox();
            this.bt_conn = new System.Windows.Forms.Button();
            this.txt_shell = new System.Windows.Forms.TextBox();
            this.cm_widget = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.moveToTabToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.addWidgetToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.removeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openConfigToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveConfigToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveAsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.importOldConfigToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.boardToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.readConfigToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.writeConfigToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.downloadFirmwareToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.ofd_loadcfg = new System.Windows.Forms.OpenFileDialog();
            this.ofd_savecfg = new System.Windows.Forms.SaveFileDialog();
            this.ofd_fwfile = new System.Windows.Forms.OpenFileDialog();
            this.pb = new System.Windows.Forms.ProgressBar();
            this.timer_com = new System.Windows.Forms.Timer(this.components);
            this.timer_submit = new System.Windows.Forms.Timer(this.components);
            this.timer_heartbeat = new System.Windows.Forms.Timer(this.components);
            this.ofd_tlog = new System.Windows.Forms.OpenFileDialog();
            this.ofd_upCfg = new System.Windows.Forms.OpenFileDialog();
            this.ofd_dnCfg = new System.Windows.Forms.SaveFileDialog();
            this.gb_shell = new System.Windows.Forms.GroupBox();
            this.button1 = new System.Windows.Forms.Button();
            this.label10 = new System.Windows.Forms.Label();
            this.cb_hwrev = new System.Windows.Forms.ComboBox();
            this.cb_comport = new System.Windows.Forms.ComboBox();
            this.tab_log = new System.Windows.Forms.TabPage();
            this.txt_log = new System.Windows.Forms.TextBox();
            this.tab_widgets = new System.Windows.Forms.TabPage();
            this.cb_units = new System.Windows.Forms.ComboBox();
            this.label32 = new System.Windows.Forms.Label();
            this.bt_refreshCanvas = new System.Windows.Forms.Button();
            this.tb_wp4 = new System.Windows.Forms.TextBox();
            this.tb_wp3 = new System.Windows.Forms.TextBox();
            this.tb_wp2 = new System.Windows.Forms.TextBox();
            this.tb_wp1 = new System.Windows.Forms.TextBox();
            this.cb_wsource = new System.Windows.Forms.ComboBox();
            this.cb_wunits = new System.Windows.Forms.ComboBox();
            this.cb_wmode = new System.Windows.Forms.ComboBox();
            this.cb_whjust = new System.Windows.Forms.ComboBox();
            this.cb_wvjust = new System.Windows.Forms.ComboBox();
            this.nud_wypos = new System.Windows.Forms.NumericUpDown();
            this.nud_wxpos = new System.Windows.Forms.NumericUpDown();
            this.lbl_wp4 = new System.Windows.Forms.Label();
            this.lbl_wp3 = new System.Windows.Forms.Label();
            this.lbl_wp2 = new System.Windows.Forms.Label();
            this.lbl_wp1 = new System.Windows.Forms.Label();
            this.lbl_wsource = new System.Windows.Forms.Label();
            this.label20 = new System.Windows.Forms.Label();
            this.lbl_wmode = new System.Windows.Forms.Label();
            this.label18 = new System.Windows.Forms.Label();
            this.label17 = new System.Windows.Forms.Label();
            this.label16 = new System.Windows.Forms.Label();
            this.label15 = new System.Windows.Forms.Label();
            this.lbl_wname = new System.Windows.Forms.Label();
            this.pb_osd = new System.Windows.Forms.PictureBox();
            this.lb_widgets = new System.Windows.Forms.ListBox();
            this.nud_seltab = new System.Windows.Forms.NumericUpDown();
            this.label13 = new System.Windows.Forms.Label();
            this.tab_misc = new System.Windows.Forms.TabPage();
            this.gb_rssi = new System.Windows.Forms.GroupBox();
            this.nud_rssi_max = new System.Windows.Forms.NumericUpDown();
            this.nud_rssi_min = new System.Windows.Forms.NumericUpDown();
            this.cb_rssi_units = new System.Windows.Forms.ComboBox();
            this.cb_rssi_src = new System.Windows.Forms.ComboBox();
            this.label63 = new System.Windows.Forms.Label();
            this.label62 = new System.Windows.Forms.Label();
            this.label61 = new System.Windows.Forms.Label();
            this.label31 = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.nud_tabmax = new System.Windows.Forms.NumericUpDown();
            this.nud_tabmin = new System.Windows.Forms.NumericUpDown();
            this.label30 = new System.Windows.Forms.Label();
            this.label29 = new System.Windows.Forms.Label();
            this.cb_tabch = new System.Windows.Forms.ComboBox();
            this.nud_tabtimer = new System.Windows.Forms.NumericUpDown();
            this.label28 = new System.Windows.Forms.Label();
            this.label27 = new System.Windows.Forms.Label();
            this.cb_tabmode = new System.Windows.Forms.ComboBox();
            this.label26 = new System.Windows.Forms.Label();
            this.tab_alarms = new System.Windows.Forms.TabPage();
            this.bt_fa_savealarm = new System.Windows.Forms.Button();
            this.bt_fa_del = new System.Windows.Forms.Button();
            this.bt_fa_add = new System.Windows.Forms.Button();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.tb_fa_value = new System.Windows.Forms.TextBox();
            this.lbl_fa_name = new System.Windows.Forms.Label();
            this.label54 = new System.Windows.Forms.Label();
            this.label50 = new System.Windows.Forms.Label();
            this.nud_fa_timer = new System.Windows.Forms.NumericUpDown();
            this.cb_fa_type = new System.Windows.Forms.ComboBox();
            this.label48 = new System.Windows.Forms.Label();
            this.label47 = new System.Windows.Forms.Label();
            this.lb_fa_cfg = new System.Windows.Forms.ListBox();
            this.label6 = new System.Windows.Forms.Label();
            this.lb_fa = new System.Windows.Forms.ListBox();
            this.tab_mavlink = new System.Windows.Forms.TabPage();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.cbx_mavhb = new System.Windows.Forms.CheckBox();
            this.label45 = new System.Windows.Forms.Label();
            this.nud_streamExtra3 = new System.Windows.Forms.NumericUpDown();
            this.label44 = new System.Windows.Forms.Label();
            this.nud_streamExtra2 = new System.Windows.Forms.NumericUpDown();
            this.label43 = new System.Windows.Forms.Label();
            this.nud_streamExtra1 = new System.Windows.Forms.NumericUpDown();
            this.label42 = new System.Windows.Forms.Label();
            this.nud_streamPosition = new System.Windows.Forms.NumericUpDown();
            this.label41 = new System.Windows.Forms.Label();
            this.nud_streamRawCtrl = new System.Windows.Forms.NumericUpDown();
            this.label40 = new System.Windows.Forms.Label();
            this.nud_streamRcChannels = new System.Windows.Forms.NumericUpDown();
            this.label39 = new System.Windows.Forms.Label();
            this.nud_streamExtStatus = new System.Windows.Forms.NumericUpDown();
            this.label38 = new System.Windows.Forms.Label();
            this.nud_streamRawSensors = new System.Windows.Forms.NumericUpDown();
            this.label37 = new System.Windows.Forms.Label();
            this.nud_osdsysid = new System.Windows.Forms.NumericUpDown();
            this.nud_uavsysid = new System.Windows.Forms.NumericUpDown();
            this.label34 = new System.Windows.Forms.Label();
            this.label33 = new System.Windows.Forms.Label();
            this.tab_uarts = new System.Windows.Forms.TabPage();
            this.gb_uart4 = new System.Windows.Forms.GroupBox();
            this.label23 = new System.Windows.Forms.Label();
            this.label24 = new System.Windows.Forms.Label();
            this.cb_baud4 = new System.Windows.Forms.ComboBox();
            this.cb_mode4 = new System.Windows.Forms.ComboBox();
            this.gb_uart3 = new System.Windows.Forms.GroupBox();
            this.label19 = new System.Windows.Forms.Label();
            this.label21 = new System.Windows.Forms.Label();
            this.cb_baud3 = new System.Windows.Forms.ComboBox();
            this.cb_mode3 = new System.Windows.Forms.ComboBox();
            this.gb_uart2 = new System.Windows.Forms.GroupBox();
            this.cb_baud2 = new System.Windows.Forms.ComboBox();
            this.label12 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.cb_mode2 = new System.Windows.Forms.ComboBox();
            this.gb_uart1 = new System.Windows.Forms.GroupBox();
            this.label9 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.cb_baud1 = new System.Windows.Forms.ComboBox();
            this.cb_mode1 = new System.Windows.Forms.ComboBox();
            this.pb_uarthw = new System.Windows.Forms.PictureBox();
            this.tab_video = new System.Windows.Forms.TabPage();
            this.gb_vsync = new System.Windows.Forms.GroupBox();
            this.label66 = new System.Windows.Forms.Label();
            this.nud_vsync1 = new System.Windows.Forms.NumericUpDown();
            this.label65 = new System.Windows.Forms.Label();
            this.nud_vsync0 = new System.Windows.Forms.NumericUpDown();
            this.label64 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.nud_whitelvl = new System.Windows.Forms.NumericUpDown();
            this.label36 = new System.Windows.Forms.Label();
            this.nud_graylvl = new System.Windows.Forms.NumericUpDown();
            this.label35 = new System.Windows.Forms.Label();
            this.nud_blacklvl = new System.Windows.Forms.NumericUpDown();
            this.label25 = new System.Windows.Forms.Label();
            this.gb_vid0v4 = new System.Windows.Forms.GroupBox();
            this.nud_vswmax = new System.Windows.Forms.NumericUpDown();
            this.nud_vswmin = new System.Windows.Forms.NumericUpDown();
            this.label55 = new System.Windows.Forms.Label();
            this.label56 = new System.Windows.Forms.Label();
            this.cb_vswch = new System.Windows.Forms.ComboBox();
            this.nud_vswtimer = new System.Windows.Forms.NumericUpDown();
            this.label57 = new System.Windows.Forms.Label();
            this.label58 = new System.Windows.Forms.Label();
            this.cb_vswmode = new System.Windows.Forms.ComboBox();
            this.label59 = new System.Windows.Forms.Label();
            this.groupBox8 = new System.Windows.Forms.GroupBox();
            this.label46 = new System.Windows.Forms.Label();
            this.cb_vidscan1 = new System.Windows.Forms.ComboBox();
            this.cb_xsize1 = new System.Windows.Forms.ComboBox();
            this.label49 = new System.Windows.Forms.Label();
            this.label51 = new System.Windows.Forms.Label();
            this.label52 = new System.Windows.Forms.Label();
            this.nud_ysize1 = new System.Windows.Forms.NumericUpDown();
            this.label53 = new System.Windows.Forms.Label();
            this.nud_xoffset1 = new System.Windows.Forms.NumericUpDown();
            this.nud_yoffset1 = new System.Windows.Forms.NumericUpDown();
            this.groupBox7 = new System.Windows.Forms.GroupBox();
            this.label1 = new System.Windows.Forms.Label();
            this.cb_vidscan = new System.Windows.Forms.ComboBox();
            this.cb_xsize = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.nud_ysize = new System.Windows.Forms.NumericUpDown();
            this.nud_xoffset = new System.Windows.Forms.NumericUpDown();
            this.nud_yoffset = new System.Windows.Forms.NumericUpDown();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.timer_submit_cfg = new System.Windows.Forms.Timer(this.components);
            this.bt_flash_fw = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.tb_tlog)).BeginInit();
            this.cm_widget.SuspendLayout();
            this.menuStrip1.SuspendLayout();
            this.gb_shell.SuspendLayout();
            this.tab_log.SuspendLayout();
            this.tab_widgets.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nud_wypos)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_wxpos)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pb_osd)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_seltab)).BeginInit();
            this.tab_misc.SuspendLayout();
            this.gb_rssi.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nud_rssi_max)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_rssi_min)).BeginInit();
            this.groupBox3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nud_tabmax)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_tabmin)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_tabtimer)).BeginInit();
            this.tab_alarms.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nud_fa_timer)).BeginInit();
            this.tab_mavlink.SuspendLayout();
            this.groupBox4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nud_streamExtra3)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_streamExtra2)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_streamExtra1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_streamPosition)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_streamRawCtrl)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_streamRcChannels)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_streamExtStatus)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_streamRawSensors)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_osdsysid)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_uavsysid)).BeginInit();
            this.tab_uarts.SuspendLayout();
            this.gb_uart4.SuspendLayout();
            this.gb_uart3.SuspendLayout();
            this.gb_uart2.SuspendLayout();
            this.gb_uart1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pb_uarthw)).BeginInit();
            this.tab_video.SuspendLayout();
            this.gb_vsync.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nud_vsync1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_vsync0)).BeginInit();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nud_whitelvl)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_graylvl)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_blacklvl)).BeginInit();
            this.gb_vid0v4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nud_vswmax)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_vswmin)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_vswtimer)).BeginInit();
            this.groupBox8.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nud_ysize1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_xoffset1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_yoffset1)).BeginInit();
            this.groupBox7.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nud_ysize)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_xoffset)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_yoffset)).BeginInit();
            this.tabControl1.SuspendLayout();
            this.SuspendLayout();
            // 
            // bt_dnCfg
            // 
            this.bt_dnCfg.Location = new System.Drawing.Point(380, 48);
            this.bt_dnCfg.Name = "bt_dnCfg";
            this.bt_dnCfg.Size = new System.Drawing.Size(106, 23);
            this.bt_dnCfg.TabIndex = 11;
            this.bt_dnCfg.Text = "Download config...";
            this.bt_dnCfg.UseVisualStyleBackColor = true;
            this.bt_dnCfg.Click += new System.EventHandler(this.bt_dnCfg_Click);
            // 
            // bt_upCfg
            // 
            this.bt_upCfg.Location = new System.Drawing.Point(380, 19);
            this.bt_upCfg.Name = "bt_upCfg";
            this.bt_upCfg.Size = new System.Drawing.Size(106, 23);
            this.bt_upCfg.TabIndex = 10;
            this.bt_upCfg.Text = "Upload config...";
            this.bt_upCfg.UseVisualStyleBackColor = true;
            this.bt_upCfg.Click += new System.EventHandler(this.bt_upCfg_Click);
            // 
            // tb_tlog
            // 
            this.tb_tlog.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_tlog.BackColor = System.Drawing.SystemColors.Window;
            this.tb_tlog.Location = new System.Drawing.Point(870, 48);
            this.tb_tlog.Maximum = 100;
            this.tb_tlog.Name = "tb_tlog";
            this.tb_tlog.Size = new System.Drawing.Size(129, 45);
            this.tb_tlog.SmallChange = 5;
            this.tb_tlog.TabIndex = 9;
            this.tb_tlog.TickFrequency = 5;
            this.tb_tlog.Scroll += new System.EventHandler(this.tb_tlog_Scroll);
            // 
            // bt_sendTlog
            // 
            this.bt_sendTlog.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.bt_sendTlog.Enabled = false;
            this.bt_sendTlog.Location = new System.Drawing.Point(876, 19);
            this.bt_sendTlog.Name = "bt_sendTlog";
            this.bt_sendTlog.Size = new System.Drawing.Size(120, 23);
            this.bt_sendTlog.TabIndex = 8;
            this.bt_sendTlog.Text = "Send .tlog";
            this.bt_sendTlog.UseVisualStyleBackColor = true;
            this.bt_sendTlog.Click += new System.EventHandler(this.bt_sendTlog_Click);
            // 
            // bt_reboot
            // 
            this.bt_reboot.Location = new System.Drawing.Point(299, 19);
            this.bt_reboot.Name = "bt_reboot";
            this.bt_reboot.Size = new System.Drawing.Size(75, 23);
            this.bt_reboot.TabIndex = 7;
            this.bt_reboot.Text = "Reboot";
            this.bt_reboot.UseVisualStyleBackColor = true;
            this.bt_reboot.Click += new System.EventHandler(this.bt_reboot_Click);
            // 
            // cbx_mavmode
            // 
            this.cbx_mavmode.AutoSize = true;
            this.cbx_mavmode.Checked = true;
            this.cbx_mavmode.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cbx_mavmode.Location = new System.Drawing.Point(170, 23);
            this.cbx_mavmode.Name = "cbx_mavmode";
            this.cbx_mavmode.Size = new System.Drawing.Size(92, 17);
            this.cbx_mavmode.TabIndex = 6;
            this.cbx_mavmode.Text = "Mavlink mode";
            this.cbx_mavmode.UseVisualStyleBackColor = true;
            // 
            // label60
            // 
            this.label60.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.label60.AutoSize = true;
            this.label60.Location = new System.Drawing.Point(1002, 16);
            this.label60.Name = "label60";
            this.label60.Size = new System.Drawing.Size(87, 13);
            this.label60.TabIndex = 5;
            this.label60.Text = "Command history";
            // 
            // bt_submitCfg
            // 
            this.bt_submitCfg.Location = new System.Drawing.Point(492, 19);
            this.bt_submitCfg.Name = "bt_submitCfg";
            this.bt_submitCfg.Size = new System.Drawing.Size(116, 23);
            this.bt_submitCfg.TabIndex = 4;
            this.bt_submitCfg.Text = "Save config";
            this.bt_submitCfg.UseVisualStyleBackColor = true;
            this.bt_submitCfg.Click += new System.EventHandler(this.bt_submitCfg_Click);
            // 
            // lb_history
            // 
            this.lb_history.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.lb_history.FormattingEnabled = true;
            this.lb_history.Location = new System.Drawing.Point(1005, 32);
            this.lb_history.Name = "lb_history";
            this.lb_history.Size = new System.Drawing.Size(162, 251);
            this.lb_history.TabIndex = 3;
            this.lb_history.TabStop = false;
            this.lb_history.DoubleClick += new System.EventHandler(this.lb_history_DoubleClick);
            // 
            // tb_cmdLine
            // 
            this.tb_cmdLine.AcceptsReturn = true;
            this.tb_cmdLine.AcceptsTab = true;
            this.tb_cmdLine.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tb_cmdLine.BackColor = System.Drawing.SystemColors.Window;
            this.tb_cmdLine.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tb_cmdLine.ForeColor = System.Drawing.SystemColors.WindowText;
            this.tb_cmdLine.Location = new System.Drawing.Point(6, 294);
            this.tb_cmdLine.Multiline = true;
            this.tb_cmdLine.Name = "tb_cmdLine";
            this.tb_cmdLine.Size = new System.Drawing.Size(1161, 28);
            this.tb_cmdLine.TabIndex = 2;
            this.tb_cmdLine.KeyDown += new System.Windows.Forms.KeyEventHandler(this.tb_cmdLine_KeyDown);
            this.tb_cmdLine.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.tb_cmdLine_KeyPress);
            // 
            // bt_conn
            // 
            this.bt_conn.Location = new System.Drawing.Point(6, 19);
            this.bt_conn.Name = "bt_conn";
            this.bt_conn.Size = new System.Drawing.Size(75, 23);
            this.bt_conn.TabIndex = 1;
            this.bt_conn.Text = "Connect";
            this.bt_conn.UseVisualStyleBackColor = true;
            this.bt_conn.Click += new System.EventHandler(this.bt_conn_Click);
            // 
            // txt_shell
            // 
            this.txt_shell.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txt_shell.BackColor = System.Drawing.SystemColors.WindowText;
            this.txt_shell.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txt_shell.ForeColor = System.Drawing.SystemColors.Window;
            this.txt_shell.Location = new System.Drawing.Point(6, 74);
            this.txt_shell.Multiline = true;
            this.txt_shell.Name = "txt_shell";
            this.txt_shell.ReadOnly = true;
            this.txt_shell.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txt_shell.Size = new System.Drawing.Size(993, 214);
            this.txt_shell.TabIndex = 0;
            this.txt_shell.VisibleChanged += new System.EventHandler(this.txt_shell_VisibleChanged);
            // 
            // cm_widget
            // 
            this.cm_widget.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.moveToTabToolStripMenuItem,
            this.addWidgetToolStripMenuItem,
            this.removeToolStripMenuItem});
            this.cm_widget.Name = "cm_widget";
            this.cm_widget.Size = new System.Drawing.Size(148, 70);
            this.cm_widget.Opening += new System.ComponentModel.CancelEventHandler(this.cm_widget_Opening);
            // 
            // moveToTabToolStripMenuItem
            // 
            this.moveToTabToolStripMenuItem.Name = "moveToTabToolStripMenuItem";
            this.moveToTabToolStripMenuItem.Size = new System.Drawing.Size(147, 22);
            this.moveToTabToolStripMenuItem.Text = "Move to tab...";
            this.moveToTabToolStripMenuItem.Click += new System.EventHandler(this.moveToTabToolStripMenuItem_Click);
            // 
            // addWidgetToolStripMenuItem
            // 
            this.addWidgetToolStripMenuItem.Name = "addWidgetToolStripMenuItem";
            this.addWidgetToolStripMenuItem.Size = new System.Drawing.Size(147, 22);
            this.addWidgetToolStripMenuItem.Text = "Add new";
            this.addWidgetToolStripMenuItem.Click += new System.EventHandler(this.addWidgetToolStripMenuItem_Click);
            // 
            // removeToolStripMenuItem
            // 
            this.removeToolStripMenuItem.Name = "removeToolStripMenuItem";
            this.removeToolStripMenuItem.Size = new System.Drawing.Size(147, 22);
            this.removeToolStripMenuItem.Text = "Remove";
            this.removeToolStripMenuItem.Click += new System.EventHandler(this.removeToolStripMenuItem_Click);
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.boardToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(1197, 24);
            this.menuStrip1.TabIndex = 1;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openConfigToolStripMenuItem,
            this.saveConfigToolStripMenuItem,
            this.saveAsToolStripMenuItem,
            this.importOldConfigToolStripMenuItem,
            this.toolStripSeparator1,
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // openConfigToolStripMenuItem
            // 
            this.openConfigToolStripMenuItem.Name = "openConfigToolStripMenuItem";
            this.openConfigToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O)));
            this.openConfigToolStripMenuItem.Size = new System.Drawing.Size(192, 22);
            this.openConfigToolStripMenuItem.Text = "Open config...";
            this.openConfigToolStripMenuItem.Click += new System.EventHandler(this.openConfigToolStripMenuItem_Click);
            // 
            // saveConfigToolStripMenuItem
            // 
            this.saveConfigToolStripMenuItem.Name = "saveConfigToolStripMenuItem";
            this.saveConfigToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.S)));
            this.saveConfigToolStripMenuItem.Size = new System.Drawing.Size(192, 22);
            this.saveConfigToolStripMenuItem.Text = "Save config";
            this.saveConfigToolStripMenuItem.Click += new System.EventHandler(this.saveConfigToolStripMenuItem_Click);
            // 
            // saveAsToolStripMenuItem
            // 
            this.saveAsToolStripMenuItem.Name = "saveAsToolStripMenuItem";
            this.saveAsToolStripMenuItem.Size = new System.Drawing.Size(192, 22);
            this.saveAsToolStripMenuItem.Text = "Save config as...";
            this.saveAsToolStripMenuItem.Click += new System.EventHandler(this.saveAsToolStripMenuItem_Click);
            // 
            // importOldConfigToolStripMenuItem
            // 
            this.importOldConfigToolStripMenuItem.Name = "importOldConfigToolStripMenuItem";
            this.importOldConfigToolStripMenuItem.Size = new System.Drawing.Size(192, 22);
            this.importOldConfigToolStripMenuItem.Text = "Import old config...";
            this.importOldConfigToolStripMenuItem.Click += new System.EventHandler(this.importOldConfigToolStripMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(189, 6);
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(192, 22);
            this.exitToolStripMenuItem.Text = "Exit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // boardToolStripMenuItem
            // 
            this.boardToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.readConfigToolStripMenuItem,
            this.writeConfigToolStripMenuItem,
            this.toolStripSeparator2,
            this.downloadFirmwareToolStripMenuItem});
            this.boardToolStripMenuItem.Name = "boardToolStripMenuItem";
            this.boardToolStripMenuItem.Size = new System.Drawing.Size(50, 20);
            this.boardToolStripMenuItem.Text = "Board";
            // 
            // readConfigToolStripMenuItem
            // 
            this.readConfigToolStripMenuItem.Name = "readConfigToolStripMenuItem";
            this.readConfigToolStripMenuItem.Size = new System.Drawing.Size(193, 22);
            this.readConfigToolStripMenuItem.Text = "Download config (old)";
            this.readConfigToolStripMenuItem.Click += new System.EventHandler(this.readConfigToolStripMenuItem_Click);
            // 
            // writeConfigToolStripMenuItem
            // 
            this.writeConfigToolStripMenuItem.Name = "writeConfigToolStripMenuItem";
            this.writeConfigToolStripMenuItem.Size = new System.Drawing.Size(193, 22);
            this.writeConfigToolStripMenuItem.Text = "Upload old config...";
            this.writeConfigToolStripMenuItem.Click += new System.EventHandler(this.writeConfigToolStripMenuItem_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(190, 6);
            // 
            // downloadFirmwareToolStripMenuItem
            // 
            this.downloadFirmwareToolStripMenuItem.Name = "downloadFirmwareToolStripMenuItem";
            this.downloadFirmwareToolStripMenuItem.Size = new System.Drawing.Size(193, 22);
            this.downloadFirmwareToolStripMenuItem.Text = "Download firmware...";
            this.downloadFirmwareToolStripMenuItem.Click += new System.EventHandler(this.downloadFirmwareToolStripMenuItem_Click);
            // 
            // ofd_loadcfg
            // 
            this.ofd_loadcfg.Filter = "Config files|*.txt";
            // 
            // ofd_fwfile
            // 
            this.ofd_fwfile.Filter = "Intel Hex|*.hex|All files|*.*";
            // 
            // pb
            // 
            this.pb.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.pb.Location = new System.Drawing.Point(12, 866);
            this.pb.Name = "pb";
            this.pb.Size = new System.Drawing.Size(1173, 23);
            this.pb.TabIndex = 5;
            // 
            // timer_com
            // 
            this.timer_com.Tick += new System.EventHandler(this.timer_com_Tick);
            // 
            // timer_submit
            // 
            this.timer_submit.Interval = 500;
            this.timer_submit.Tick += new System.EventHandler(this.timer_submit_Tick);
            // 
            // timer_heartbeat
            // 
            this.timer_heartbeat.Interval = 1000;
            this.timer_heartbeat.Tick += new System.EventHandler(this.timer_heartbeat_Tick);
            // 
            // ofd_tlog
            // 
            this.ofd_tlog.FileName = ".tlog";
            this.ofd_tlog.Filter = "Telemetry log files|*.tlog";
            // 
            // ofd_upCfg
            // 
            this.ofd_upCfg.DefaultExt = "txt";
            this.ofd_upCfg.FileName = "alceosd_config";
            this.ofd_upCfg.Filter = "Text files|*.txt|All files|*.*";
            // 
            // ofd_dnCfg
            // 
            this.ofd_dnCfg.DefaultExt = "txt";
            this.ofd_dnCfg.FileName = "alceosd_config";
            this.ofd_dnCfg.Filter = "Text files|*.txt|All files|*.*";
            // 
            // gb_shell
            // 
            this.gb_shell.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.gb_shell.BackColor = System.Drawing.SystemColors.Window;
            this.gb_shell.Controls.Add(this.bt_flash_fw);
            this.gb_shell.Controls.Add(this.button1);
            this.gb_shell.Controls.Add(this.label10);
            this.gb_shell.Controls.Add(this.cb_hwrev);
            this.gb_shell.Controls.Add(this.cb_comport);
            this.gb_shell.Controls.Add(this.cbx_mavmode);
            this.gb_shell.Controls.Add(this.bt_conn);
            this.gb_shell.Controls.Add(this.bt_reboot);
            this.gb_shell.Controls.Add(this.bt_upCfg);
            this.gb_shell.Controls.Add(this.label60);
            this.gb_shell.Controls.Add(this.txt_shell);
            this.gb_shell.Controls.Add(this.tb_cmdLine);
            this.gb_shell.Controls.Add(this.lb_history);
            this.gb_shell.Controls.Add(this.bt_dnCfg);
            this.gb_shell.Controls.Add(this.tb_tlog);
            this.gb_shell.Controls.Add(this.bt_sendTlog);
            this.gb_shell.Controls.Add(this.bt_submitCfg);
            this.gb_shell.Location = new System.Drawing.Point(12, 532);
            this.gb_shell.Name = "gb_shell";
            this.gb_shell.Size = new System.Drawing.Size(1173, 328);
            this.gb_shell.TabIndex = 7;
            this.gb_shell.TabStop = false;
            this.gb_shell.Text = "Console";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(626, 19);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 21);
            this.button1.TabIndex = 15;
            this.button1.Text = "button1";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(13, 53);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(68, 13);
            this.label10.TabIndex = 14;
            this.label10.Text = "AlceOSD hw";
            // 
            // cb_hwrev
            // 
            this.cb_hwrev.FormattingEnabled = true;
            this.cb_hwrev.Items.AddRange(new object[] {
            "0v1",
            "0v2",
            "0v3",
            "0v3b",
            "0v4b",
            "0v5"});
            this.cb_hwrev.Location = new System.Drawing.Point(87, 48);
            this.cb_hwrev.Name = "cb_hwrev";
            this.cb_hwrev.Size = new System.Drawing.Size(77, 21);
            this.cb_hwrev.TabIndex = 13;
            this.cb_hwrev.SelectedIndexChanged += new System.EventHandler(this.cb_hwrev_SelectedIndexChanged);
            // 
            // cb_comport
            // 
            this.cb_comport.FormattingEnabled = true;
            this.cb_comport.Location = new System.Drawing.Point(87, 21);
            this.cb_comport.Name = "cb_comport";
            this.cb_comport.Size = new System.Drawing.Size(77, 21);
            this.cb_comport.TabIndex = 12;
            this.cb_comport.Click += new System.EventHandler(this.cb_comport_Click);
            // 
            // tab_log
            // 
            this.tab_log.Controls.Add(this.txt_log);
            this.tab_log.Location = new System.Drawing.Point(4, 22);
            this.tab_log.Name = "tab_log";
            this.tab_log.Padding = new System.Windows.Forms.Padding(3);
            this.tab_log.Size = new System.Drawing.Size(1165, 477);
            this.tab_log.TabIndex = 4;
            this.tab_log.Text = "Log";
            this.tab_log.UseVisualStyleBackColor = true;
            // 
            // txt_log
            // 
            this.txt_log.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txt_log.BackColor = System.Drawing.SystemColors.Window;
            this.txt_log.ForeColor = System.Drawing.SystemColors.WindowText;
            this.txt_log.Location = new System.Drawing.Point(6, 6);
            this.txt_log.Multiline = true;
            this.txt_log.Name = "txt_log";
            this.txt_log.ReadOnly = true;
            this.txt_log.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txt_log.Size = new System.Drawing.Size(1153, 465);
            this.txt_log.TabIndex = 0;
            // 
            // tab_widgets
            // 
            this.tab_widgets.Controls.Add(this.cb_units);
            this.tab_widgets.Controls.Add(this.label32);
            this.tab_widgets.Controls.Add(this.bt_refreshCanvas);
            this.tab_widgets.Controls.Add(this.tb_wp4);
            this.tab_widgets.Controls.Add(this.tb_wp3);
            this.tab_widgets.Controls.Add(this.tb_wp2);
            this.tab_widgets.Controls.Add(this.tb_wp1);
            this.tab_widgets.Controls.Add(this.cb_wsource);
            this.tab_widgets.Controls.Add(this.cb_wunits);
            this.tab_widgets.Controls.Add(this.cb_wmode);
            this.tab_widgets.Controls.Add(this.cb_whjust);
            this.tab_widgets.Controls.Add(this.cb_wvjust);
            this.tab_widgets.Controls.Add(this.nud_wypos);
            this.tab_widgets.Controls.Add(this.nud_wxpos);
            this.tab_widgets.Controls.Add(this.lbl_wp4);
            this.tab_widgets.Controls.Add(this.lbl_wp3);
            this.tab_widgets.Controls.Add(this.lbl_wp2);
            this.tab_widgets.Controls.Add(this.lbl_wp1);
            this.tab_widgets.Controls.Add(this.lbl_wsource);
            this.tab_widgets.Controls.Add(this.label20);
            this.tab_widgets.Controls.Add(this.lbl_wmode);
            this.tab_widgets.Controls.Add(this.label18);
            this.tab_widgets.Controls.Add(this.label17);
            this.tab_widgets.Controls.Add(this.label16);
            this.tab_widgets.Controls.Add(this.label15);
            this.tab_widgets.Controls.Add(this.lbl_wname);
            this.tab_widgets.Controls.Add(this.pb_osd);
            this.tab_widgets.Controls.Add(this.lb_widgets);
            this.tab_widgets.Controls.Add(this.nud_seltab);
            this.tab_widgets.Controls.Add(this.label13);
            this.tab_widgets.Location = new System.Drawing.Point(4, 22);
            this.tab_widgets.Name = "tab_widgets";
            this.tab_widgets.Padding = new System.Windows.Forms.Padding(3);
            this.tab_widgets.Size = new System.Drawing.Size(1165, 477);
            this.tab_widgets.TabIndex = 3;
            this.tab_widgets.Text = "Widgets";
            this.tab_widgets.UseVisualStyleBackColor = true;
            // 
            // cb_units
            // 
            this.cb_units.FormattingEnabled = true;
            this.cb_units.Items.AddRange(new object[] {
            "Metric",
            "Imperial"});
            this.cb_units.Location = new System.Drawing.Point(464, 439);
            this.cb_units.Name = "cb_units";
            this.cb_units.Size = new System.Drawing.Size(92, 21);
            this.cb_units.TabIndex = 7;
            this.cb_units.Text = "Metric";
            this.cb_units.SelectedIndexChanged += new System.EventHandler(this.cb_units_SelectedIndexChanged);
            // 
            // label32
            // 
            this.label32.AutoSize = true;
            this.label32.Location = new System.Drawing.Point(353, 442);
            this.label32.Name = "label32";
            this.label32.Size = new System.Drawing.Size(105, 13);
            this.label32.TabIndex = 6;
            this.label32.Text = "Default Widget Units";
            // 
            // bt_refreshCanvas
            // 
            this.bt_refreshCanvas.Location = new System.Drawing.Point(210, 437);
            this.bt_refreshCanvas.Name = "bt_refreshCanvas";
            this.bt_refreshCanvas.Size = new System.Drawing.Size(137, 23);
            this.bt_refreshCanvas.TabIndex = 8;
            this.bt_refreshCanvas.Text = "Reload OSD";
            this.bt_refreshCanvas.UseVisualStyleBackColor = true;
            this.bt_refreshCanvas.Click += new System.EventHandler(this.bt_refreshCanvas_Click);
            // 
            // tb_wp4
            // 
            this.tb_wp4.Location = new System.Drawing.Point(352, 354);
            this.tb_wp4.Name = "tb_wp4";
            this.tb_wp4.Size = new System.Drawing.Size(100, 20);
            this.tb_wp4.TabIndex = 6;
            this.tb_wp4.TextChanged += new System.EventHandler(this.tb_wp4_TextChanged);
            // 
            // tb_wp3
            // 
            this.tb_wp3.Location = new System.Drawing.Point(352, 328);
            this.tb_wp3.Name = "tb_wp3";
            this.tb_wp3.Size = new System.Drawing.Size(100, 20);
            this.tb_wp3.TabIndex = 6;
            this.tb_wp3.TextChanged += new System.EventHandler(this.tb_wp3_TextChanged);
            // 
            // tb_wp2
            // 
            this.tb_wp2.Location = new System.Drawing.Point(352, 302);
            this.tb_wp2.Name = "tb_wp2";
            this.tb_wp2.Size = new System.Drawing.Size(100, 20);
            this.tb_wp2.TabIndex = 6;
            this.tb_wp2.TextChanged += new System.EventHandler(this.tb_wp2_TextChanged);
            // 
            // tb_wp1
            // 
            this.tb_wp1.Location = new System.Drawing.Point(352, 276);
            this.tb_wp1.Name = "tb_wp1";
            this.tb_wp1.Size = new System.Drawing.Size(100, 20);
            this.tb_wp1.TabIndex = 6;
            this.tb_wp1.TextChanged += new System.EventHandler(this.tb_wp1_TextChanged);
            // 
            // cb_wsource
            // 
            this.cb_wsource.FormattingEnabled = true;
            this.cb_wsource.Location = new System.Drawing.Point(352, 249);
            this.cb_wsource.Name = "cb_wsource";
            this.cb_wsource.Size = new System.Drawing.Size(195, 21);
            this.cb_wsource.TabIndex = 5;
            this.cb_wsource.SelectedIndexChanged += new System.EventHandler(this.cb_wsource_SelectedIndexChanged);
            // 
            // cb_wunits
            // 
            this.cb_wunits.FormattingEnabled = true;
            this.cb_wunits.Location = new System.Drawing.Point(352, 222);
            this.cb_wunits.Name = "cb_wunits";
            this.cb_wunits.Size = new System.Drawing.Size(195, 21);
            this.cb_wunits.TabIndex = 5;
            this.cb_wunits.SelectedIndexChanged += new System.EventHandler(this.cb_wunits_SelectedIndexChanged);
            // 
            // cb_wmode
            // 
            this.cb_wmode.FormattingEnabled = true;
            this.cb_wmode.Location = new System.Drawing.Point(352, 195);
            this.cb_wmode.Name = "cb_wmode";
            this.cb_wmode.Size = new System.Drawing.Size(195, 21);
            this.cb_wmode.TabIndex = 5;
            this.cb_wmode.SelectedIndexChanged += new System.EventHandler(this.cb_wmode_SelectedIndexChanged);
            // 
            // cb_whjust
            // 
            this.cb_whjust.FormattingEnabled = true;
            this.cb_whjust.Items.AddRange(new object[] {
            "Left",
            "Right",
            "Center"});
            this.cb_whjust.Location = new System.Drawing.Point(352, 168);
            this.cb_whjust.Name = "cb_whjust";
            this.cb_whjust.Size = new System.Drawing.Size(100, 21);
            this.cb_whjust.TabIndex = 5;
            this.cb_whjust.Text = "Top";
            this.cb_whjust.SelectedIndexChanged += new System.EventHandler(this.cb_whjust_SelectedIndexChanged);
            // 
            // cb_wvjust
            // 
            this.cb_wvjust.FormattingEnabled = true;
            this.cb_wvjust.Items.AddRange(new object[] {
            "Top",
            "Bottom",
            "Center"});
            this.cb_wvjust.Location = new System.Drawing.Point(352, 141);
            this.cb_wvjust.Name = "cb_wvjust";
            this.cb_wvjust.Size = new System.Drawing.Size(100, 21);
            this.cb_wvjust.TabIndex = 5;
            this.cb_wvjust.Text = "Top";
            this.cb_wvjust.SelectedIndexChanged += new System.EventHandler(this.cb_wvjust_SelectedIndexChanged);
            // 
            // nud_wypos
            // 
            this.nud_wypos.Location = new System.Drawing.Point(352, 115);
            this.nud_wypos.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.nud_wypos.Minimum = new decimal(new int[] {
            1000,
            0,
            0,
            -2147483648});
            this.nud_wypos.Name = "nud_wypos";
            this.nud_wypos.Size = new System.Drawing.Size(100, 20);
            this.nud_wypos.TabIndex = 4;
            this.nud_wypos.ValueChanged += new System.EventHandler(this.nud_wypos_ValueChanged);
            // 
            // nud_wxpos
            // 
            this.nud_wxpos.Increment = new decimal(new int[] {
            4,
            0,
            0,
            0});
            this.nud_wxpos.Location = new System.Drawing.Point(352, 89);
            this.nud_wxpos.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.nud_wxpos.Minimum = new decimal(new int[] {
            1000,
            0,
            0,
            -2147483648});
            this.nud_wxpos.Name = "nud_wxpos";
            this.nud_wxpos.Size = new System.Drawing.Size(100, 20);
            this.nud_wxpos.TabIndex = 4;
            this.nud_wxpos.ValueChanged += new System.EventHandler(this.nud_wxpos_ValueChanged);
            // 
            // lbl_wp4
            // 
            this.lbl_wp4.AutoSize = true;
            this.lbl_wp4.Location = new System.Drawing.Point(207, 357);
            this.lbl_wp4.Name = "lbl_wp4";
            this.lbl_wp4.Size = new System.Drawing.Size(43, 13);
            this.lbl_wp4.TabIndex = 3;
            this.lbl_wp4.Text = "Param4";
            // 
            // lbl_wp3
            // 
            this.lbl_wp3.AutoSize = true;
            this.lbl_wp3.Location = new System.Drawing.Point(207, 331);
            this.lbl_wp3.Name = "lbl_wp3";
            this.lbl_wp3.Size = new System.Drawing.Size(43, 13);
            this.lbl_wp3.TabIndex = 3;
            this.lbl_wp3.Text = "Param3";
            // 
            // lbl_wp2
            // 
            this.lbl_wp2.AutoSize = true;
            this.lbl_wp2.Location = new System.Drawing.Point(207, 305);
            this.lbl_wp2.Name = "lbl_wp2";
            this.lbl_wp2.Size = new System.Drawing.Size(43, 13);
            this.lbl_wp2.TabIndex = 3;
            this.lbl_wp2.Text = "Param2";
            // 
            // lbl_wp1
            // 
            this.lbl_wp1.AutoSize = true;
            this.lbl_wp1.Location = new System.Drawing.Point(207, 279);
            this.lbl_wp1.Name = "lbl_wp1";
            this.lbl_wp1.Size = new System.Drawing.Size(43, 13);
            this.lbl_wp1.TabIndex = 3;
            this.lbl_wp1.Text = "Param1";
            // 
            // lbl_wsource
            // 
            this.lbl_wsource.AutoSize = true;
            this.lbl_wsource.Location = new System.Drawing.Point(207, 252);
            this.lbl_wsource.Name = "lbl_wsource";
            this.lbl_wsource.Size = new System.Drawing.Size(41, 13);
            this.lbl_wsource.TabIndex = 3;
            this.lbl_wsource.Text = "Source";
            // 
            // label20
            // 
            this.label20.AutoSize = true;
            this.label20.Location = new System.Drawing.Point(207, 225);
            this.label20.Name = "label20";
            this.label20.Size = new System.Drawing.Size(31, 13);
            this.label20.TabIndex = 3;
            this.label20.Text = "Units";
            // 
            // lbl_wmode
            // 
            this.lbl_wmode.AutoSize = true;
            this.lbl_wmode.Location = new System.Drawing.Point(207, 198);
            this.lbl_wmode.Name = "lbl_wmode";
            this.lbl_wmode.Size = new System.Drawing.Size(34, 13);
            this.lbl_wmode.TabIndex = 3;
            this.lbl_wmode.Text = "Mode";
            // 
            // label18
            // 
            this.label18.AutoSize = true;
            this.label18.Location = new System.Drawing.Point(207, 171);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(109, 13);
            this.label18.TabIndex = 3;
            this.label18.Text = "Horizontal justification";
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Location = new System.Drawing.Point(207, 144);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(97, 13);
            this.label17.TabIndex = 3;
            this.label17.Text = "Vertical justification";
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point(207, 117);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(53, 13);
            this.label16.TabIndex = 3;
            this.label16.Text = "Y position";
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(207, 91);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(53, 13);
            this.label15.TabIndex = 3;
            this.label15.Text = "X position";
            // 
            // lbl_wname
            // 
            this.lbl_wname.AutoSize = true;
            this.lbl_wname.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lbl_wname.Location = new System.Drawing.Point(207, 55);
            this.lbl_wname.Name = "lbl_wname";
            this.lbl_wname.Size = new System.Drawing.Size(81, 13);
            this.lbl_wname.TabIndex = 3;
            this.lbl_wname.Text = "Widget name";
            // 
            // pb_osd
            // 
            this.pb_osd.BackColor = System.Drawing.Color.DimGray;
            this.pb_osd.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.pb_osd.Location = new System.Drawing.Point(565, 36);
            this.pb_osd.Name = "pb_osd";
            this.pb_osd.Size = new System.Drawing.Size(52, 43);
            this.pb_osd.TabIndex = 7;
            this.pb_osd.TabStop = false;
            this.pb_osd.Paint += new System.Windows.Forms.PaintEventHandler(this.pb_osd_Paint);
            this.pb_osd.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.pb_osd_MouseDoubleClick);
            this.pb_osd.MouseDown += new System.Windows.Forms.MouseEventHandler(this.pb_osd_MouseDown);
            this.pb_osd.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pb_osd_MouseMove);
            this.pb_osd.MouseUp += new System.Windows.Forms.MouseEventHandler(this.pb_osd_MouseUp);
            // 
            // lb_widgets
            // 
            this.lb_widgets.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
            this.lb_widgets.ContextMenuStrip = this.cm_widget;
            this.lb_widgets.FormattingEnabled = true;
            this.lb_widgets.Location = new System.Drawing.Point(18, 38);
            this.lb_widgets.Name = "lb_widgets";
            this.lb_widgets.Size = new System.Drawing.Size(156, 394);
            this.lb_widgets.TabIndex = 2;
            this.lb_widgets.SelectedIndexChanged += new System.EventHandler(this.lb_widgets_SelectedIndexChanged);
            this.lb_widgets.DoubleClick += new System.EventHandler(this.lb_widgets_DoubleClick);
            // 
            // nud_seltab
            // 
            this.nud_seltab.Location = new System.Drawing.Point(47, 12);
            this.nud_seltab.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.nud_seltab.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nud_seltab.Name = "nud_seltab";
            this.nud_seltab.Size = new System.Drawing.Size(48, 20);
            this.nud_seltab.TabIndex = 1;
            this.nud_seltab.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nud_seltab.ValueChanged += new System.EventHandler(this.nud_seltab_ValueChanged);
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(15, 14);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(26, 13);
            this.label13.TabIndex = 0;
            this.label13.Text = "Tab";
            // 
            // tab_misc
            // 
            this.tab_misc.Controls.Add(this.gb_rssi);
            this.tab_misc.Controls.Add(this.groupBox3);
            this.tab_misc.Location = new System.Drawing.Point(4, 22);
            this.tab_misc.Name = "tab_misc";
            this.tab_misc.Padding = new System.Windows.Forms.Padding(3);
            this.tab_misc.Size = new System.Drawing.Size(1165, 477);
            this.tab_misc.TabIndex = 2;
            this.tab_misc.Text = "Misc";
            this.tab_misc.UseVisualStyleBackColor = true;
            // 
            // gb_rssi
            // 
            this.gb_rssi.Controls.Add(this.nud_rssi_max);
            this.gb_rssi.Controls.Add(this.nud_rssi_min);
            this.gb_rssi.Controls.Add(this.cb_rssi_units);
            this.gb_rssi.Controls.Add(this.cb_rssi_src);
            this.gb_rssi.Controls.Add(this.label63);
            this.gb_rssi.Controls.Add(this.label62);
            this.gb_rssi.Controls.Add(this.label61);
            this.gb_rssi.Controls.Add(this.label31);
            this.gb_rssi.Location = new System.Drawing.Point(295, 6);
            this.gb_rssi.Name = "gb_rssi";
            this.gb_rssi.Size = new System.Drawing.Size(243, 162);
            this.gb_rssi.TabIndex = 8;
            this.gb_rssi.TabStop = false;
            this.gb_rssi.Text = "RSSI";
            // 
            // nud_rssi_max
            // 
            this.nud_rssi_max.Location = new System.Drawing.Point(86, 99);
            this.nud_rssi_max.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.nud_rssi_max.Name = "nud_rssi_max";
            this.nud_rssi_max.Size = new System.Drawing.Size(102, 20);
            this.nud_rssi_max.TabIndex = 7;
            this.nud_rssi_max.ValueChanged += new System.EventHandler(this.rssi_config_changed);
            // 
            // nud_rssi_min
            // 
            this.nud_rssi_min.Location = new System.Drawing.Point(86, 73);
            this.nud_rssi_min.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.nud_rssi_min.Name = "nud_rssi_min";
            this.nud_rssi_min.Size = new System.Drawing.Size(102, 20);
            this.nud_rssi_min.TabIndex = 6;
            this.nud_rssi_min.ValueChanged += new System.EventHandler(this.rssi_config_changed);
            // 
            // cb_rssi_units
            // 
            this.cb_rssi_units.FormattingEnabled = true;
            this.cb_rssi_units.Items.AddRange(new object[] {
            "Percent",
            "Raw"});
            this.cb_rssi_units.Location = new System.Drawing.Point(86, 46);
            this.cb_rssi_units.Name = "cb_rssi_units";
            this.cb_rssi_units.Size = new System.Drawing.Size(102, 21);
            this.cb_rssi_units.TabIndex = 5;
            this.cb_rssi_units.Text = "Percent";
            this.cb_rssi_units.SelectedIndexChanged += new System.EventHandler(this.rssi_config_changed);
            // 
            // cb_rssi_src
            // 
            this.cb_rssi_src.FormattingEnabled = true;
            this.cb_rssi_src.Items.AddRange(new object[] {
            "RC1",
            "RC2",
            "RC3",
            "RC4",
            "RC5",
            "RC6",
            "RC7",
            "RC8",
            "RC9",
            "RC10",
            "RC11",
            "RC12",
            "RC13",
            "RC14",
            "RC15",
            "RC16",
            "RC17",
            "RC18",
            "ADC in0",
            "ADC in1",
            "Mavlink RSSI"});
            this.cb_rssi_src.Location = new System.Drawing.Point(86, 19);
            this.cb_rssi_src.Name = "cb_rssi_src";
            this.cb_rssi_src.Size = new System.Drawing.Size(102, 21);
            this.cb_rssi_src.TabIndex = 4;
            this.cb_rssi_src.Text = "Mavlink RSSI";
            this.cb_rssi_src.SelectedIndexChanged += new System.EventHandler(this.rssi_config_changed);
            // 
            // label63
            // 
            this.label63.AutoSize = true;
            this.label63.Location = new System.Drawing.Point(6, 101);
            this.label63.Name = "label63";
            this.label63.Size = new System.Drawing.Size(51, 13);
            this.label63.TabIndex = 3;
            this.label63.Text = "Maximum";
            // 
            // label62
            // 
            this.label62.AutoSize = true;
            this.label62.Location = new System.Drawing.Point(6, 75);
            this.label62.Name = "label62";
            this.label62.Size = new System.Drawing.Size(46, 13);
            this.label62.TabIndex = 2;
            this.label62.Text = "Mininum";
            // 
            // label61
            // 
            this.label61.AutoSize = true;
            this.label61.Location = new System.Drawing.Point(6, 48);
            this.label61.Name = "label61";
            this.label61.Size = new System.Drawing.Size(31, 13);
            this.label61.TabIndex = 1;
            this.label61.Text = "Units";
            // 
            // label31
            // 
            this.label31.AutoSize = true;
            this.label31.Location = new System.Drawing.Point(6, 22);
            this.label31.Name = "label31";
            this.label31.Size = new System.Drawing.Size(41, 13);
            this.label31.TabIndex = 0;
            this.label31.Text = "Source";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.nud_tabmax);
            this.groupBox3.Controls.Add(this.nud_tabmin);
            this.groupBox3.Controls.Add(this.label30);
            this.groupBox3.Controls.Add(this.label29);
            this.groupBox3.Controls.Add(this.cb_tabch);
            this.groupBox3.Controls.Add(this.nud_tabtimer);
            this.groupBox3.Controls.Add(this.label28);
            this.groupBox3.Controls.Add(this.label27);
            this.groupBox3.Controls.Add(this.cb_tabmode);
            this.groupBox3.Controls.Add(this.label26);
            this.groupBox3.Location = new System.Drawing.Point(6, 6);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(283, 162);
            this.groupBox3.TabIndex = 3;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Tab Switching";
            // 
            // nud_tabmax
            // 
            this.nud_tabmax.Location = new System.Drawing.Point(107, 125);
            this.nud_tabmax.Maximum = new decimal(new int[] {
            2200,
            0,
            0,
            0});
            this.nud_tabmax.Minimum = new decimal(new int[] {
            800,
            0,
            0,
            0});
            this.nud_tabmax.Name = "nud_tabmax";
            this.nud_tabmax.Size = new System.Drawing.Size(93, 20);
            this.nud_tabmax.TabIndex = 11;
            this.nud_tabmax.Value = new decimal(new int[] {
            2000,
            0,
            0,
            0});
            this.nud_tabmax.ValueChanged += new System.EventHandler(this.tabs_config_changed);
            // 
            // nud_tabmin
            // 
            this.nud_tabmin.Location = new System.Drawing.Point(107, 99);
            this.nud_tabmin.Maximum = new decimal(new int[] {
            2200,
            0,
            0,
            0});
            this.nud_tabmin.Minimum = new decimal(new int[] {
            800,
            0,
            0,
            0});
            this.nud_tabmin.Name = "nud_tabmin";
            this.nud_tabmin.Size = new System.Drawing.Size(93, 20);
            this.nud_tabmin.TabIndex = 10;
            this.nud_tabmin.Value = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.nud_tabmin.ValueChanged += new System.EventHandler(this.tabs_config_changed);
            // 
            // label30
            // 
            this.label30.AutoSize = true;
            this.label30.Location = new System.Drawing.Point(15, 127);
            this.label30.Name = "label30";
            this.label30.Size = new System.Drawing.Size(74, 13);
            this.label30.TabIndex = 9;
            this.label30.Text = "CH PWM max";
            // 
            // label29
            // 
            this.label29.AutoSize = true;
            this.label29.Location = new System.Drawing.Point(15, 101);
            this.label29.Name = "label29";
            this.label29.Size = new System.Drawing.Size(71, 13);
            this.label29.TabIndex = 8;
            this.label29.Text = "CH PWM min";
            // 
            // cb_tabch
            // 
            this.cb_tabch.FormattingEnabled = true;
            this.cb_tabch.Items.AddRange(new object[] {
            "RC1",
            "RC2",
            "RC3",
            "RC4",
            "RC5",
            "RC6",
            "RC7",
            "RC8"});
            this.cb_tabch.Location = new System.Drawing.Point(107, 72);
            this.cb_tabch.Name = "cb_tabch";
            this.cb_tabch.Size = new System.Drawing.Size(93, 21);
            this.cb_tabch.TabIndex = 7;
            this.cb_tabch.Text = "RC8";
            this.cb_tabch.SelectedIndexChanged += new System.EventHandler(this.tabs_config_changed);
            // 
            // nud_tabtimer
            // 
            this.nud_tabtimer.Increment = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.nud_tabtimer.Location = new System.Drawing.Point(107, 46);
            this.nud_tabtimer.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.nud_tabtimer.Minimum = new decimal(new int[] {
            500,
            0,
            0,
            0});
            this.nud_tabtimer.Name = "nud_tabtimer";
            this.nud_tabtimer.Size = new System.Drawing.Size(93, 20);
            this.nud_tabtimer.TabIndex = 6;
            this.nud_tabtimer.Value = new decimal(new int[] {
            2000,
            0,
            0,
            0});
            this.nud_tabtimer.ValueChanged += new System.EventHandler(this.tabs_config_changed);
            // 
            // label28
            // 
            this.label28.AutoSize = true;
            this.label28.Location = new System.Drawing.Point(15, 75);
            this.label28.Name = "label28";
            this.label28.Size = new System.Drawing.Size(46, 13);
            this.label28.TabIndex = 5;
            this.label28.Text = "Channel";
            // 
            // label27
            // 
            this.label27.AutoSize = true;
            this.label27.Location = new System.Drawing.Point(15, 48);
            this.label27.Name = "label27";
            this.label27.Size = new System.Drawing.Size(55, 13);
            this.label27.TabIndex = 5;
            this.label27.Text = "Timer (ms)";
            // 
            // cb_tabmode
            // 
            this.cb_tabmode.FormattingEnabled = true;
            this.cb_tabmode.Items.AddRange(new object[] {
            "Percentage",
            "Flight mode",
            "Toggle",
            "Demo"});
            this.cb_tabmode.Location = new System.Drawing.Point(107, 19);
            this.cb_tabmode.Name = "cb_tabmode";
            this.cb_tabmode.Size = new System.Drawing.Size(93, 21);
            this.cb_tabmode.TabIndex = 4;
            this.cb_tabmode.Text = "Percentage";
            this.cb_tabmode.SelectedIndexChanged += new System.EventHandler(this.tabs_config_changed);
            // 
            // label26
            // 
            this.label26.AutoSize = true;
            this.label26.Location = new System.Drawing.Point(15, 22);
            this.label26.Name = "label26";
            this.label26.Size = new System.Drawing.Size(34, 13);
            this.label26.TabIndex = 3;
            this.label26.Text = "Mode";
            // 
            // tab_alarms
            // 
            this.tab_alarms.Controls.Add(this.bt_fa_savealarm);
            this.tab_alarms.Controls.Add(this.bt_fa_del);
            this.tab_alarms.Controls.Add(this.bt_fa_add);
            this.tab_alarms.Controls.Add(this.textBox1);
            this.tab_alarms.Controls.Add(this.tb_fa_value);
            this.tab_alarms.Controls.Add(this.lbl_fa_name);
            this.tab_alarms.Controls.Add(this.label54);
            this.tab_alarms.Controls.Add(this.label50);
            this.tab_alarms.Controls.Add(this.nud_fa_timer);
            this.tab_alarms.Controls.Add(this.cb_fa_type);
            this.tab_alarms.Controls.Add(this.label48);
            this.tab_alarms.Controls.Add(this.label47);
            this.tab_alarms.Controls.Add(this.lb_fa_cfg);
            this.tab_alarms.Controls.Add(this.label6);
            this.tab_alarms.Controls.Add(this.lb_fa);
            this.tab_alarms.Location = new System.Drawing.Point(4, 22);
            this.tab_alarms.Name = "tab_alarms";
            this.tab_alarms.Padding = new System.Windows.Forms.Padding(3);
            this.tab_alarms.Size = new System.Drawing.Size(1165, 477);
            this.tab_alarms.TabIndex = 7;
            this.tab_alarms.Text = "Alarms";
            this.tab_alarms.UseVisualStyleBackColor = true;
            // 
            // bt_fa_savealarm
            // 
            this.bt_fa_savealarm.Location = new System.Drawing.Point(22, 352);
            this.bt_fa_savealarm.Name = "bt_fa_savealarm";
            this.bt_fa_savealarm.Size = new System.Drawing.Size(177, 23);
            this.bt_fa_savealarm.TabIndex = 15;
            this.bt_fa_savealarm.Text = "Submit changes";
            this.bt_fa_savealarm.UseVisualStyleBackColor = true;
            this.bt_fa_savealarm.Click += new System.EventHandler(this.bt_fa_savealarm_Click);
            // 
            // bt_fa_del
            // 
            this.bt_fa_del.Location = new System.Drawing.Point(157, 119);
            this.bt_fa_del.Name = "bt_fa_del";
            this.bt_fa_del.Size = new System.Drawing.Size(28, 23);
            this.bt_fa_del.TabIndex = 14;
            this.bt_fa_del.Text = "<";
            this.bt_fa_del.UseVisualStyleBackColor = true;
            this.bt_fa_del.Click += new System.EventHandler(this.bt_fa_del_Click);
            // 
            // bt_fa_add
            // 
            this.bt_fa_add.Location = new System.Drawing.Point(157, 90);
            this.bt_fa_add.Name = "bt_fa_add";
            this.bt_fa_add.Size = new System.Drawing.Size(28, 23);
            this.bt_fa_add.TabIndex = 13;
            this.bt_fa_add.Text = ">";
            this.bt_fa_add.UseVisualStyleBackColor = true;
            this.bt_fa_add.Click += new System.EventHandler(this.bt_fa_add_Click);
            // 
            // textBox1
            // 
            this.textBox1.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.textBox1.Location = new System.Drawing.Point(205, 271);
            this.textBox1.Margin = new System.Windows.Forms.Padding(0);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(251, 151);
            this.textBox1.TabIndex = 12;
            this.textBox1.Text = resources.GetString("textBox1.Text");
            // 
            // tb_fa_value
            // 
            this.tb_fa_value.Location = new System.Drawing.Point(78, 326);
            this.tb_fa_value.Name = "tb_fa_value";
            this.tb_fa_value.Size = new System.Drawing.Size(120, 20);
            this.tb_fa_value.TabIndex = 9;
            // 
            // lbl_fa_name
            // 
            this.lbl_fa_name.AutoSize = true;
            this.lbl_fa_name.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lbl_fa_name.Location = new System.Drawing.Point(19, 231);
            this.lbl_fa_name.Name = "lbl_fa_name";
            this.lbl_fa_name.Size = new System.Drawing.Size(20, 16);
            this.lbl_fa_name.TabIndex = 11;
            this.lbl_fa_name.Text = "   ";
            // 
            // label54
            // 
            this.label54.AutoSize = true;
            this.label54.Location = new System.Drawing.Point(16, 329);
            this.label54.Name = "label54";
            this.label54.Size = new System.Drawing.Size(34, 13);
            this.label54.TabIndex = 8;
            this.label54.Text = "Value";
            // 
            // label50
            // 
            this.label50.AutoSize = true;
            this.label50.Location = new System.Drawing.Point(16, 301);
            this.label50.Name = "label50";
            this.label50.Size = new System.Drawing.Size(56, 13);
            this.label50.TabIndex = 7;
            this.label50.Text = "Delay (ms)";
            // 
            // nud_fa_timer
            // 
            this.nud_fa_timer.Increment = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.nud_fa_timer.Location = new System.Drawing.Point(78, 299);
            this.nud_fa_timer.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.nud_fa_timer.Name = "nud_fa_timer";
            this.nud_fa_timer.Size = new System.Drawing.Size(120, 20);
            this.nud_fa_timer.TabIndex = 6;
            // 
            // cb_fa_type
            // 
            this.cb_fa_type.FormattingEnabled = true;
            this.cb_fa_type.Items.AddRange(new object[] {
            "High",
            "Low"});
            this.cb_fa_type.Location = new System.Drawing.Point(78, 271);
            this.cb_fa_type.Name = "cb_fa_type";
            this.cb_fa_type.Size = new System.Drawing.Size(121, 21);
            this.cb_fa_type.TabIndex = 5;
            // 
            // label48
            // 
            this.label48.AutoSize = true;
            this.label48.Location = new System.Drawing.Point(16, 274);
            this.label48.Name = "label48";
            this.label48.Size = new System.Drawing.Size(56, 13);
            this.label48.TabIndex = 4;
            this.label48.Text = "Alarm type";
            // 
            // label47
            // 
            this.label47.AutoSize = true;
            this.label47.Location = new System.Drawing.Point(191, 17);
            this.label47.Name = "label47";
            this.label47.Size = new System.Drawing.Size(92, 13);
            this.label47.TabIndex = 3;
            this.label47.Text = "Configured Alarms";
            // 
            // lb_fa_cfg
            // 
            this.lb_fa_cfg.FormattingEnabled = true;
            this.lb_fa_cfg.Location = new System.Drawing.Point(191, 33);
            this.lb_fa_cfg.Name = "lb_fa_cfg";
            this.lb_fa_cfg.Size = new System.Drawing.Size(251, 173);
            this.lb_fa_cfg.TabIndex = 2;
            this.lb_fa_cfg.SelectedIndexChanged += new System.EventHandler(this.lb_fa_cfg_SelectedIndexChanged);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(16, 17);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(84, 13);
            this.label6.TabIndex = 1;
            this.label6.Text = "Available Alarms";
            // 
            // lb_fa
            // 
            this.lb_fa.FormattingEnabled = true;
            this.lb_fa.Items.AddRange(new object[] {
            "Battery voltage (mavlink)",
            "Battery voltage (AN0)",
            "Battery voltage (AN1)",
            "RSSI (mavlink)",
            "Home distance",
            "Air speed",
            "Home Altitude",
            "GPS sattelites"});
            this.lb_fa.Location = new System.Drawing.Point(19, 33);
            this.lb_fa.Name = "lb_fa";
            this.lb_fa.Size = new System.Drawing.Size(132, 173);
            this.lb_fa.TabIndex = 0;
            // 
            // tab_mavlink
            // 
            this.tab_mavlink.Controls.Add(this.groupBox4);
            this.tab_mavlink.Location = new System.Drawing.Point(4, 22);
            this.tab_mavlink.Name = "tab_mavlink";
            this.tab_mavlink.Size = new System.Drawing.Size(1165, 477);
            this.tab_mavlink.TabIndex = 6;
            this.tab_mavlink.Text = "Mavlink";
            this.tab_mavlink.UseVisualStyleBackColor = true;
            // 
            // groupBox4
            // 
            this.groupBox4.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox4.Controls.Add(this.cbx_mavhb);
            this.groupBox4.Controls.Add(this.label45);
            this.groupBox4.Controls.Add(this.nud_streamExtra3);
            this.groupBox4.Controls.Add(this.label44);
            this.groupBox4.Controls.Add(this.nud_streamExtra2);
            this.groupBox4.Controls.Add(this.label43);
            this.groupBox4.Controls.Add(this.nud_streamExtra1);
            this.groupBox4.Controls.Add(this.label42);
            this.groupBox4.Controls.Add(this.nud_streamPosition);
            this.groupBox4.Controls.Add(this.label41);
            this.groupBox4.Controls.Add(this.nud_streamRawCtrl);
            this.groupBox4.Controls.Add(this.label40);
            this.groupBox4.Controls.Add(this.nud_streamRcChannels);
            this.groupBox4.Controls.Add(this.label39);
            this.groupBox4.Controls.Add(this.nud_streamExtStatus);
            this.groupBox4.Controls.Add(this.label38);
            this.groupBox4.Controls.Add(this.nud_streamRawSensors);
            this.groupBox4.Controls.Add(this.label37);
            this.groupBox4.Controls.Add(this.nud_osdsysid);
            this.groupBox4.Controls.Add(this.nud_uavsysid);
            this.groupBox4.Controls.Add(this.label34);
            this.groupBox4.Controls.Add(this.label33);
            this.groupBox4.Location = new System.Drawing.Point(3, 3);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(1090, 486);
            this.groupBox4.TabIndex = 9;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Mavlink";
            // 
            // cbx_mavhb
            // 
            this.cbx_mavhb.AutoSize = true;
            this.cbx_mavhb.Checked = true;
            this.cbx_mavhb.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cbx_mavhb.Location = new System.Drawing.Point(18, 87);
            this.cbx_mavhb.Name = "cbx_mavhb";
            this.cbx_mavhb.Size = new System.Drawing.Size(139, 17);
            this.cbx_mavhb.TabIndex = 9;
            this.cbx_mavhb.Text = "OSD Mavlink Heartbeat";
            this.cbx_mavhb.UseVisualStyleBackColor = true;
            this.cbx_mavhb.CheckedChanged += new System.EventHandler(this.mavlink_config_changed);
            // 
            // label45
            // 
            this.label45.AutoSize = true;
            this.label45.Location = new System.Drawing.Point(265, 10);
            this.label45.Name = "label45";
            this.label45.Size = new System.Drawing.Size(71, 13);
            this.label45.TabIndex = 27;
            this.label45.Text = "Stream Rates";
            // 
            // nud_streamExtra3
            // 
            this.nud_streamExtra3.Location = new System.Drawing.Point(362, 212);
            this.nud_streamExtra3.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.nud_streamExtra3.Name = "nud_streamExtra3";
            this.nud_streamExtra3.Size = new System.Drawing.Size(93, 20);
            this.nud_streamExtra3.TabIndex = 26;
            this.nud_streamExtra3.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nud_streamExtra3.ValueChanged += new System.EventHandler(this.nud_streamExtra3_ValueChanged);
            // 
            // label44
            // 
            this.label44.AutoSize = true;
            this.label44.Location = new System.Drawing.Point(265, 214);
            this.label44.Name = "label44";
            this.label44.Size = new System.Drawing.Size(49, 13);
            this.label44.TabIndex = 25;
            this.label44.Text = "EXTRA3";
            // 
            // nud_streamExtra2
            // 
            this.nud_streamExtra2.Location = new System.Drawing.Point(362, 186);
            this.nud_streamExtra2.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.nud_streamExtra2.Name = "nud_streamExtra2";
            this.nud_streamExtra2.Size = new System.Drawing.Size(93, 20);
            this.nud_streamExtra2.TabIndex = 24;
            this.nud_streamExtra2.Value = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.nud_streamExtra2.ValueChanged += new System.EventHandler(this.nud_streamExtra2_ValueChanged);
            // 
            // label43
            // 
            this.label43.AutoSize = true;
            this.label43.Location = new System.Drawing.Point(265, 188);
            this.label43.Name = "label43";
            this.label43.Size = new System.Drawing.Size(49, 13);
            this.label43.TabIndex = 23;
            this.label43.Text = "EXTRA2";
            // 
            // nud_streamExtra1
            // 
            this.nud_streamExtra1.Location = new System.Drawing.Point(362, 160);
            this.nud_streamExtra1.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.nud_streamExtra1.Name = "nud_streamExtra1";
            this.nud_streamExtra1.Size = new System.Drawing.Size(93, 20);
            this.nud_streamExtra1.TabIndex = 22;
            this.nud_streamExtra1.Value = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.nud_streamExtra1.ValueChanged += new System.EventHandler(this.nud_streamExtra1_ValueChanged);
            // 
            // label42
            // 
            this.label42.AutoSize = true;
            this.label42.Location = new System.Drawing.Point(265, 162);
            this.label42.Name = "label42";
            this.label42.Size = new System.Drawing.Size(49, 13);
            this.label42.TabIndex = 21;
            this.label42.Text = "EXTRA1";
            // 
            // nud_streamPosition
            // 
            this.nud_streamPosition.Location = new System.Drawing.Point(362, 134);
            this.nud_streamPosition.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.nud_streamPosition.Name = "nud_streamPosition";
            this.nud_streamPosition.Size = new System.Drawing.Size(93, 20);
            this.nud_streamPosition.TabIndex = 20;
            this.nud_streamPosition.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nud_streamPosition.ValueChanged += new System.EventHandler(this.nud_streamPosition_ValueChanged);
            // 
            // label41
            // 
            this.label41.AutoSize = true;
            this.label41.Location = new System.Drawing.Point(265, 136);
            this.label41.Name = "label41";
            this.label41.Size = new System.Drawing.Size(58, 13);
            this.label41.TabIndex = 19;
            this.label41.Text = "POSITION";
            // 
            // nud_streamRawCtrl
            // 
            this.nud_streamRawCtrl.Location = new System.Drawing.Point(362, 108);
            this.nud_streamRawCtrl.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.nud_streamRawCtrl.Name = "nud_streamRawCtrl";
            this.nud_streamRawCtrl.Size = new System.Drawing.Size(93, 20);
            this.nud_streamRawCtrl.TabIndex = 18;
            this.nud_streamRawCtrl.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nud_streamRawCtrl.ValueChanged += new System.EventHandler(this.nud_streamRawCtrl_ValueChanged);
            // 
            // label40
            // 
            this.label40.AutoSize = true;
            this.label40.Location = new System.Drawing.Point(265, 110);
            this.label40.Name = "label40";
            this.label40.Size = new System.Drawing.Size(67, 13);
            this.label40.TabIndex = 17;
            this.label40.Text = "RAW_CTRL";
            // 
            // nud_streamRcChannels
            // 
            this.nud_streamRcChannels.Location = new System.Drawing.Point(362, 82);
            this.nud_streamRcChannels.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.nud_streamRcChannels.Name = "nud_streamRcChannels";
            this.nud_streamRcChannels.Size = new System.Drawing.Size(93, 20);
            this.nud_streamRcChannels.TabIndex = 16;
            this.nud_streamRcChannels.Value = new decimal(new int[] {
            4,
            0,
            0,
            0});
            this.nud_streamRcChannels.ValueChanged += new System.EventHandler(this.nud_streamRcChannels_ValueChanged);
            // 
            // label39
            // 
            this.label39.AutoSize = true;
            this.label39.Location = new System.Drawing.Point(265, 84);
            this.label39.Name = "label39";
            this.label39.Size = new System.Drawing.Size(86, 13);
            this.label39.TabIndex = 15;
            this.label39.Text = "RC_CHANNELS";
            // 
            // nud_streamExtStatus
            // 
            this.nud_streamExtStatus.Location = new System.Drawing.Point(362, 56);
            this.nud_streamExtStatus.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.nud_streamExtStatus.Name = "nud_streamExtStatus";
            this.nud_streamExtStatus.Size = new System.Drawing.Size(93, 20);
            this.nud_streamExtStatus.TabIndex = 14;
            this.nud_streamExtStatus.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nud_streamExtStatus.ValueChanged += new System.EventHandler(this.nud_streamExtStatus_ValueChanged);
            // 
            // label38
            // 
            this.label38.AutoSize = true;
            this.label38.Location = new System.Drawing.Point(265, 58);
            this.label38.Name = "label38";
            this.label38.Size = new System.Drawing.Size(77, 13);
            this.label38.TabIndex = 13;
            this.label38.Text = "EXT_STATUS";
            // 
            // nud_streamRawSensors
            // 
            this.nud_streamRawSensors.Location = new System.Drawing.Point(362, 30);
            this.nud_streamRawSensors.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.nud_streamRawSensors.Name = "nud_streamRawSensors";
            this.nud_streamRawSensors.Size = new System.Drawing.Size(93, 20);
            this.nud_streamRawSensors.TabIndex = 12;
            this.nud_streamRawSensors.Value = new decimal(new int[] {
            3,
            0,
            0,
            0});
            this.nud_streamRawSensors.ValueChanged += new System.EventHandler(this.nud_streamRawSensors_ValueChanged);
            // 
            // label37
            // 
            this.label37.AutoSize = true;
            this.label37.Location = new System.Drawing.Point(265, 32);
            this.label37.Name = "label37";
            this.label37.Size = new System.Drawing.Size(91, 13);
            this.label37.TabIndex = 11;
            this.label37.Text = "RAW_SENSORS";
            // 
            // nud_osdsysid
            // 
            this.nud_osdsysid.Location = new System.Drawing.Point(112, 51);
            this.nud_osdsysid.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.nud_osdsysid.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nud_osdsysid.Name = "nud_osdsysid";
            this.nud_osdsysid.Size = new System.Drawing.Size(93, 20);
            this.nud_osdsysid.TabIndex = 10;
            this.nud_osdsysid.Value = new decimal(new int[] {
            200,
            0,
            0,
            0});
            this.nud_osdsysid.ValueChanged += new System.EventHandler(this.mavlink_config_changed);
            // 
            // nud_uavsysid
            // 
            this.nud_uavsysid.Location = new System.Drawing.Point(112, 25);
            this.nud_uavsysid.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.nud_uavsysid.Name = "nud_uavsysid";
            this.nud_uavsysid.Size = new System.Drawing.Size(93, 20);
            this.nud_uavsysid.TabIndex = 9;
            this.nud_uavsysid.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nud_uavsysid.ValueChanged += new System.EventHandler(this.mavlink_config_changed);
            // 
            // label34
            // 
            this.label34.AutoSize = true;
            this.label34.Location = new System.Drawing.Point(15, 53);
            this.label34.Name = "label34";
            this.label34.Size = new System.Drawing.Size(56, 13);
            this.label34.TabIndex = 8;
            this.label34.Text = "OSD sysid";
            // 
            // label33
            // 
            this.label33.AutoSize = true;
            this.label33.Location = new System.Drawing.Point(15, 27);
            this.label33.Name = "label33";
            this.label33.Size = new System.Drawing.Size(55, 13);
            this.label33.TabIndex = 7;
            this.label33.Text = "UAV sysid";
            // 
            // tab_uarts
            // 
            this.tab_uarts.Controls.Add(this.gb_uart4);
            this.tab_uarts.Controls.Add(this.gb_uart3);
            this.tab_uarts.Controls.Add(this.gb_uart2);
            this.tab_uarts.Controls.Add(this.gb_uart1);
            this.tab_uarts.Controls.Add(this.pb_uarthw);
            this.tab_uarts.Location = new System.Drawing.Point(4, 22);
            this.tab_uarts.Name = "tab_uarts";
            this.tab_uarts.Padding = new System.Windows.Forms.Padding(3);
            this.tab_uarts.Size = new System.Drawing.Size(1165, 477);
            this.tab_uarts.TabIndex = 1;
            this.tab_uarts.Text = "Serial ports";
            this.tab_uarts.UseVisualStyleBackColor = true;
            // 
            // gb_uart4
            // 
            this.gb_uart4.Controls.Add(this.label23);
            this.gb_uart4.Controls.Add(this.label24);
            this.gb_uart4.Controls.Add(this.cb_baud4);
            this.gb_uart4.Controls.Add(this.cb_mode4);
            this.gb_uart4.Location = new System.Drawing.Point(658, 6);
            this.gb_uart4.Name = "gb_uart4";
            this.gb_uart4.Size = new System.Drawing.Size(210, 94);
            this.gb_uart4.TabIndex = 7;
            this.gb_uart4.TabStop = false;
            this.gb_uart4.Text = "UART4";
            // 
            // label23
            // 
            this.label23.AutoSize = true;
            this.label23.Location = new System.Drawing.Point(15, 58);
            this.label23.Name = "label23";
            this.label23.Size = new System.Drawing.Size(50, 13);
            this.label23.TabIndex = 2;
            this.label23.Text = "Baudrate";
            // 
            // label24
            // 
            this.label24.AutoSize = true;
            this.label24.Location = new System.Drawing.Point(15, 31);
            this.label24.Name = "label24";
            this.label24.Size = new System.Drawing.Size(34, 13);
            this.label24.TabIndex = 2;
            this.label24.Text = "Mode";
            // 
            // cb_baud4
            // 
            this.cb_baud4.FormattingEnabled = true;
            this.cb_baud4.Items.AddRange(new object[] {
            "19200",
            "57600",
            "115200",
            "921600"});
            this.cb_baud4.Location = new System.Drawing.Point(76, 55);
            this.cb_baud4.Name = "cb_baud4";
            this.cb_baud4.Size = new System.Drawing.Size(121, 21);
            this.cb_baud4.TabIndex = 3;
            this.cb_baud4.Text = "115200";
            this.cb_baud4.SelectedIndexChanged += new System.EventHandler(this.uart4_config_changed);
            // 
            // cb_mode4
            // 
            this.cb_mode4.FormattingEnabled = true;
            this.cb_mode4.Items.AddRange(new object[] {
            "Off",
            "Mavlink",
            "UAVTalk",
            "Shell",
            "FrSky"});
            this.cb_mode4.Location = new System.Drawing.Point(76, 28);
            this.cb_mode4.Name = "cb_mode4";
            this.cb_mode4.Size = new System.Drawing.Size(121, 21);
            this.cb_mode4.TabIndex = 3;
            this.cb_mode4.Text = "Disabled";
            this.cb_mode4.SelectedIndexChanged += new System.EventHandler(this.uart4_config_changed);
            // 
            // gb_uart3
            // 
            this.gb_uart3.Controls.Add(this.label19);
            this.gb_uart3.Controls.Add(this.label21);
            this.gb_uart3.Controls.Add(this.cb_baud3);
            this.gb_uart3.Controls.Add(this.cb_mode3);
            this.gb_uart3.Location = new System.Drawing.Point(442, 6);
            this.gb_uart3.Name = "gb_uart3";
            this.gb_uart3.Size = new System.Drawing.Size(210, 94);
            this.gb_uart3.TabIndex = 6;
            this.gb_uart3.TabStop = false;
            this.gb_uart3.Text = "UART3";
            // 
            // label19
            // 
            this.label19.AutoSize = true;
            this.label19.Location = new System.Drawing.Point(15, 58);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(50, 13);
            this.label19.TabIndex = 2;
            this.label19.Text = "Baudrate";
            // 
            // label21
            // 
            this.label21.AutoSize = true;
            this.label21.Location = new System.Drawing.Point(15, 31);
            this.label21.Name = "label21";
            this.label21.Size = new System.Drawing.Size(34, 13);
            this.label21.TabIndex = 2;
            this.label21.Text = "Mode";
            // 
            // cb_baud3
            // 
            this.cb_baud3.FormattingEnabled = true;
            this.cb_baud3.Items.AddRange(new object[] {
            "19200",
            "57600",
            "115200",
            "921600"});
            this.cb_baud3.Location = new System.Drawing.Point(76, 55);
            this.cb_baud3.Name = "cb_baud3";
            this.cb_baud3.Size = new System.Drawing.Size(121, 21);
            this.cb_baud3.TabIndex = 3;
            this.cb_baud3.Text = "115200";
            this.cb_baud3.SelectedIndexChanged += new System.EventHandler(this.uart3_config_changed);
            // 
            // cb_mode3
            // 
            this.cb_mode3.FormattingEnabled = true;
            this.cb_mode3.Items.AddRange(new object[] {
            "Off",
            "Mavlink",
            "UAVTalk",
            "Shell",
            "FrSky"});
            this.cb_mode3.Location = new System.Drawing.Point(76, 28);
            this.cb_mode3.Name = "cb_mode3";
            this.cb_mode3.Size = new System.Drawing.Size(121, 21);
            this.cb_mode3.TabIndex = 3;
            this.cb_mode3.Text = "Disabled";
            this.cb_mode3.SelectedIndexChanged += new System.EventHandler(this.uart3_config_changed);
            // 
            // gb_uart2
            // 
            this.gb_uart2.Controls.Add(this.cb_baud2);
            this.gb_uart2.Controls.Add(this.label12);
            this.gb_uart2.Controls.Add(this.label8);
            this.gb_uart2.Controls.Add(this.cb_mode2);
            this.gb_uart2.Location = new System.Drawing.Point(226, 6);
            this.gb_uart2.Name = "gb_uart2";
            this.gb_uart2.Size = new System.Drawing.Size(210, 94);
            this.gb_uart2.TabIndex = 5;
            this.gb_uart2.TabStop = false;
            this.gb_uart2.Text = "UART2";
            // 
            // cb_baud2
            // 
            this.cb_baud2.FormattingEnabled = true;
            this.cb_baud2.Items.AddRange(new object[] {
            "19200",
            "57600",
            "115200",
            "921600"});
            this.cb_baud2.Location = new System.Drawing.Point(76, 55);
            this.cb_baud2.Name = "cb_baud2";
            this.cb_baud2.Size = new System.Drawing.Size(121, 21);
            this.cb_baud2.TabIndex = 9;
            this.cb_baud2.Text = "115200";
            this.cb_baud2.SelectedIndexChanged += new System.EventHandler(this.uart2_config_changed);
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(20, 58);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(50, 13);
            this.label12.TabIndex = 7;
            this.label12.Text = "Baudrate";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(20, 31);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(34, 13);
            this.label8.TabIndex = 4;
            this.label8.Text = "Mode";
            // 
            // cb_mode2
            // 
            this.cb_mode2.FormattingEnabled = true;
            this.cb_mode2.Items.AddRange(new object[] {
            "Off",
            "Mavlink",
            "UAVTalk",
            "Shell",
            "FrSky"});
            this.cb_mode2.Location = new System.Drawing.Point(76, 28);
            this.cb_mode2.Name = "cb_mode2";
            this.cb_mode2.Size = new System.Drawing.Size(121, 21);
            this.cb_mode2.TabIndex = 5;
            this.cb_mode2.Text = "Mavlink";
            this.cb_mode2.SelectedIndexChanged += new System.EventHandler(this.uart2_config_changed);
            // 
            // gb_uart1
            // 
            this.gb_uart1.Controls.Add(this.label9);
            this.gb_uart1.Controls.Add(this.label7);
            this.gb_uart1.Controls.Add(this.cb_baud1);
            this.gb_uart1.Controls.Add(this.cb_mode1);
            this.gb_uart1.Location = new System.Drawing.Point(10, 6);
            this.gb_uart1.Name = "gb_uart1";
            this.gb_uart1.Size = new System.Drawing.Size(210, 94);
            this.gb_uart1.TabIndex = 4;
            this.gb_uart1.TabStop = false;
            this.gb_uart1.Text = "UART1";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(15, 58);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(50, 13);
            this.label9.TabIndex = 2;
            this.label9.Text = "Baudrate";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(15, 31);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(34, 13);
            this.label7.TabIndex = 2;
            this.label7.Text = "Mode";
            // 
            // cb_baud1
            // 
            this.cb_baud1.FormattingEnabled = true;
            this.cb_baud1.Items.AddRange(new object[] {
            "19200",
            "57600",
            "115200",
            "921600"});
            this.cb_baud1.Location = new System.Drawing.Point(76, 55);
            this.cb_baud1.Name = "cb_baud1";
            this.cb_baud1.Size = new System.Drawing.Size(121, 21);
            this.cb_baud1.TabIndex = 3;
            this.cb_baud1.Text = "115200";
            this.cb_baud1.SelectedIndexChanged += new System.EventHandler(this.uart1_config_changed);
            // 
            // cb_mode1
            // 
            this.cb_mode1.FormattingEnabled = true;
            this.cb_mode1.Items.AddRange(new object[] {
            "Off",
            "Mavlink",
            "UAVTalk",
            "Shell",
            "FrSky"});
            this.cb_mode1.Location = new System.Drawing.Point(76, 28);
            this.cb_mode1.Name = "cb_mode1";
            this.cb_mode1.Size = new System.Drawing.Size(121, 21);
            this.cb_mode1.TabIndex = 3;
            this.cb_mode1.Text = "Mavlink";
            this.cb_mode1.SelectedIndexChanged += new System.EventHandler(this.uart1_config_changed);
            // 
            // pb_uarthw
            // 
            this.pb_uarthw.Image = global::AlceOSD.Properties.Resources.alceosd_hw0v1;
            this.pb_uarthw.Location = new System.Drawing.Point(283, 168);
            this.pb_uarthw.Name = "pb_uarthw";
            this.pb_uarthw.Size = new System.Drawing.Size(284, 241);
            this.pb_uarthw.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pb_uarthw.TabIndex = 8;
            this.pb_uarthw.TabStop = false;
            // 
            // tab_video
            // 
            this.tab_video.Controls.Add(this.gb_vsync);
            this.tab_video.Controls.Add(this.groupBox1);
            this.tab_video.Controls.Add(this.gb_vid0v4);
            this.tab_video.Controls.Add(this.groupBox8);
            this.tab_video.Controls.Add(this.groupBox7);
            this.tab_video.Location = new System.Drawing.Point(4, 22);
            this.tab_video.Name = "tab_video";
            this.tab_video.Padding = new System.Windows.Forms.Padding(3);
            this.tab_video.Size = new System.Drawing.Size(1165, 477);
            this.tab_video.TabIndex = 0;
            this.tab_video.Text = "Video";
            this.tab_video.UseVisualStyleBackColor = true;
            // 
            // gb_vsync
            // 
            this.gb_vsync.Controls.Add(this.label66);
            this.gb_vsync.Controls.Add(this.nud_vsync1);
            this.gb_vsync.Controls.Add(this.label65);
            this.gb_vsync.Controls.Add(this.nud_vsync0);
            this.gb_vsync.Controls.Add(this.label64);
            this.gb_vsync.Location = new System.Drawing.Point(308, 119);
            this.gb_vsync.Name = "gb_vsync";
            this.gb_vsync.Size = new System.Drawing.Size(296, 107);
            this.gb_vsync.TabIndex = 10;
            this.gb_vsync.TabStop = false;
            this.gb_vsync.Text = "Sync detector (hw>=0v3)";
            // 
            // label66
            // 
            this.label66.AutoSize = true;
            this.label66.Location = new System.Drawing.Point(9, 78);
            this.label66.Name = "label66";
            this.label66.Size = new System.Drawing.Size(258, 13);
            this.label66.TabIndex = 5;
            this.label66.Text = "Sync level 0 = auto sync (unstable for some cameras)";
            // 
            // nud_vsync1
            // 
            this.nud_vsync1.Location = new System.Drawing.Point(154, 50);
            this.nud_vsync1.Maximum = new decimal(new int[] {
            15,
            0,
            0,
            0});
            this.nud_vsync1.Name = "nud_vsync1";
            this.nud_vsync1.Size = new System.Drawing.Size(120, 20);
            this.nud_vsync1.TabIndex = 4;
            this.nud_vsync1.Value = new decimal(new int[] {
            4,
            0,
            0,
            0});
            this.nud_vsync1.ValueChanged += new System.EventHandler(this.video_config_changed);
            // 
            // label65
            // 
            this.label65.AutoSize = true;
            this.label65.Location = new System.Drawing.Point(9, 52);
            this.label65.Name = "label65";
            this.label65.Size = new System.Drawing.Size(79, 13);
            this.label65.TabIndex = 3;
            this.label65.Text = "Sync level (in1)";
            // 
            // nud_vsync0
            // 
            this.nud_vsync0.Location = new System.Drawing.Point(154, 24);
            this.nud_vsync0.Maximum = new decimal(new int[] {
            15,
            0,
            0,
            0});
            this.nud_vsync0.Name = "nud_vsync0";
            this.nud_vsync0.Size = new System.Drawing.Size(120, 20);
            this.nud_vsync0.TabIndex = 2;
            this.nud_vsync0.Value = new decimal(new int[] {
            4,
            0,
            0,
            0});
            this.nud_vsync0.ValueChanged += new System.EventHandler(this.video_config_changed);
            // 
            // label64
            // 
            this.label64.AutoSize = true;
            this.label64.Location = new System.Drawing.Point(9, 26);
            this.label64.Name = "label64";
            this.label64.Size = new System.Drawing.Size(79, 13);
            this.label64.TabIndex = 1;
            this.label64.Text = "Sync level (in0)";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.nud_whitelvl);
            this.groupBox1.Controls.Add(this.label36);
            this.groupBox1.Controls.Add(this.nud_graylvl);
            this.groupBox1.Controls.Add(this.label35);
            this.groupBox1.Controls.Add(this.nud_blacklvl);
            this.groupBox1.Controls.Add(this.label25);
            this.groupBox1.Location = new System.Drawing.Point(308, 6);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(296, 107);
            this.groupBox1.TabIndex = 9;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Brightness";
            // 
            // nud_whitelvl
            // 
            this.nud_whitelvl.Location = new System.Drawing.Point(153, 24);
            this.nud_whitelvl.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.nud_whitelvl.Name = "nud_whitelvl";
            this.nud_whitelvl.Size = new System.Drawing.Size(120, 20);
            this.nud_whitelvl.TabIndex = 3;
            this.nud_whitelvl.Value = new decimal(new int[] {
            63,
            0,
            0,
            0});
            this.nud_whitelvl.ValueChanged += new System.EventHandler(this.video_config_changed);
            // 
            // label36
            // 
            this.label36.AutoSize = true;
            this.label36.Location = new System.Drawing.Point(17, 78);
            this.label36.Name = "label36";
            this.label36.Size = new System.Drawing.Size(59, 13);
            this.label36.TabIndex = 5;
            this.label36.Text = "Black level";
            // 
            // nud_graylvl
            // 
            this.nud_graylvl.Location = new System.Drawing.Point(153, 50);
            this.nud_graylvl.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.nud_graylvl.Name = "nud_graylvl";
            this.nud_graylvl.Size = new System.Drawing.Size(120, 20);
            this.nud_graylvl.TabIndex = 3;
            this.nud_graylvl.Value = new decimal(new int[] {
            45,
            0,
            0,
            0});
            this.nud_graylvl.ValueChanged += new System.EventHandler(this.video_config_changed);
            // 
            // label35
            // 
            this.label35.AutoSize = true;
            this.label35.Location = new System.Drawing.Point(17, 52);
            this.label35.Name = "label35";
            this.label35.Size = new System.Drawing.Size(54, 13);
            this.label35.TabIndex = 5;
            this.label35.Text = "Gray level";
            // 
            // nud_blacklvl
            // 
            this.nud_blacklvl.Location = new System.Drawing.Point(153, 76);
            this.nud_blacklvl.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.nud_blacklvl.Name = "nud_blacklvl";
            this.nud_blacklvl.Size = new System.Drawing.Size(120, 20);
            this.nud_blacklvl.TabIndex = 3;
            this.nud_blacklvl.Value = new decimal(new int[] {
            25,
            0,
            0,
            0});
            this.nud_blacklvl.ValueChanged += new System.EventHandler(this.video_config_changed);
            // 
            // label25
            // 
            this.label25.AutoSize = true;
            this.label25.Location = new System.Drawing.Point(17, 26);
            this.label25.Name = "label25";
            this.label25.Size = new System.Drawing.Size(60, 13);
            this.label25.TabIndex = 5;
            this.label25.Text = "White level";
            // 
            // gb_vid0v4
            // 
            this.gb_vid0v4.Controls.Add(this.nud_vswmax);
            this.gb_vid0v4.Controls.Add(this.nud_vswmin);
            this.gb_vid0v4.Controls.Add(this.label55);
            this.gb_vid0v4.Controls.Add(this.label56);
            this.gb_vid0v4.Controls.Add(this.cb_vswch);
            this.gb_vid0v4.Controls.Add(this.nud_vswtimer);
            this.gb_vid0v4.Controls.Add(this.label57);
            this.gb_vid0v4.Controls.Add(this.label58);
            this.gb_vid0v4.Controls.Add(this.cb_vswmode);
            this.gb_vid0v4.Controls.Add(this.label59);
            this.gb_vid0v4.Location = new System.Drawing.Point(610, 6);
            this.gb_vid0v4.Name = "gb_vid0v4";
            this.gb_vid0v4.Size = new System.Drawing.Size(303, 164);
            this.gb_vid0v4.TabIndex = 8;
            this.gb_vid0v4.TabStop = false;
            this.gb_vid0v4.Text = "Video input (hw>=0v4)";
            // 
            // nud_vswmax
            // 
            this.nud_vswmax.Location = new System.Drawing.Point(154, 130);
            this.nud_vswmax.Maximum = new decimal(new int[] {
            2200,
            0,
            0,
            0});
            this.nud_vswmax.Minimum = new decimal(new int[] {
            800,
            0,
            0,
            0});
            this.nud_vswmax.Name = "nud_vswmax";
            this.nud_vswmax.Size = new System.Drawing.Size(120, 20);
            this.nud_vswmax.TabIndex = 21;
            this.nud_vswmax.Value = new decimal(new int[] {
            2000,
            0,
            0,
            0});
            this.nud_vswmax.ValueChanged += new System.EventHandler(this.videosw_config_changed);
            // 
            // nud_vswmin
            // 
            this.nud_vswmin.Location = new System.Drawing.Point(154, 104);
            this.nud_vswmin.Maximum = new decimal(new int[] {
            2200,
            0,
            0,
            0});
            this.nud_vswmin.Minimum = new decimal(new int[] {
            800,
            0,
            0,
            0});
            this.nud_vswmin.Name = "nud_vswmin";
            this.nud_vswmin.Size = new System.Drawing.Size(120, 20);
            this.nud_vswmin.TabIndex = 20;
            this.nud_vswmin.Value = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.nud_vswmin.ValueChanged += new System.EventHandler(this.videosw_config_changed);
            // 
            // label55
            // 
            this.label55.AutoSize = true;
            this.label55.Location = new System.Drawing.Point(9, 131);
            this.label55.Name = "label55";
            this.label55.Size = new System.Drawing.Size(74, 13);
            this.label55.TabIndex = 19;
            this.label55.Text = "CH PWM max";
            // 
            // label56
            // 
            this.label56.AutoSize = true;
            this.label56.Location = new System.Drawing.Point(9, 105);
            this.label56.Name = "label56";
            this.label56.Size = new System.Drawing.Size(71, 13);
            this.label56.TabIndex = 18;
            this.label56.Text = "CH PWM min";
            // 
            // cb_vswch
            // 
            this.cb_vswch.FormattingEnabled = true;
            this.cb_vswch.Items.AddRange(new object[] {
            "RC1",
            "RC2",
            "RC3",
            "RC4",
            "RC5",
            "RC6",
            "RC7",
            "RC8"});
            this.cb_vswch.Location = new System.Drawing.Point(154, 77);
            this.cb_vswch.Name = "cb_vswch";
            this.cb_vswch.Size = new System.Drawing.Size(120, 21);
            this.cb_vswch.TabIndex = 17;
            this.cb_vswch.Text = "RC8";
            this.cb_vswch.SelectedIndexChanged += new System.EventHandler(this.videosw_config_changed);
            // 
            // nud_vswtimer
            // 
            this.nud_vswtimer.Increment = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.nud_vswtimer.Location = new System.Drawing.Point(154, 51);
            this.nud_vswtimer.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.nud_vswtimer.Minimum = new decimal(new int[] {
            500,
            0,
            0,
            0});
            this.nud_vswtimer.Name = "nud_vswtimer";
            this.nud_vswtimer.Size = new System.Drawing.Size(120, 20);
            this.nud_vswtimer.TabIndex = 16;
            this.nud_vswtimer.Value = new decimal(new int[] {
            2000,
            0,
            0,
            0});
            this.nud_vswtimer.ValueChanged += new System.EventHandler(this.videosw_config_changed);
            // 
            // label57
            // 
            this.label57.AutoSize = true;
            this.label57.Location = new System.Drawing.Point(9, 79);
            this.label57.Name = "label57";
            this.label57.Size = new System.Drawing.Size(46, 13);
            this.label57.TabIndex = 14;
            this.label57.Text = "Channel";
            // 
            // label58
            // 
            this.label58.AutoSize = true;
            this.label58.Location = new System.Drawing.Point(9, 53);
            this.label58.Name = "label58";
            this.label58.Size = new System.Drawing.Size(55, 13);
            this.label58.TabIndex = 15;
            this.label58.Text = "Timer (ms)";
            // 
            // cb_vswmode
            // 
            this.cb_vswmode.FormattingEnabled = true;
            this.cb_vswmode.Items.AddRange(new object[] {
            "Percentage",
            "Flight mode",
            "Toggle"});
            this.cb_vswmode.Location = new System.Drawing.Point(154, 23);
            this.cb_vswmode.Name = "cb_vswmode";
            this.cb_vswmode.Size = new System.Drawing.Size(120, 21);
            this.cb_vswmode.TabIndex = 13;
            this.cb_vswmode.Text = "Percentage";
            this.cb_vswmode.SelectedIndexChanged += new System.EventHandler(this.videosw_config_changed);
            // 
            // label59
            // 
            this.label59.AutoSize = true;
            this.label59.Location = new System.Drawing.Point(9, 26);
            this.label59.Name = "label59";
            this.label59.Size = new System.Drawing.Size(34, 13);
            this.label59.TabIndex = 12;
            this.label59.Text = "Mode";
            // 
            // groupBox8
            // 
            this.groupBox8.Controls.Add(this.label46);
            this.groupBox8.Controls.Add(this.cb_vidscan1);
            this.groupBox8.Controls.Add(this.cb_xsize1);
            this.groupBox8.Controls.Add(this.label49);
            this.groupBox8.Controls.Add(this.label51);
            this.groupBox8.Controls.Add(this.label52);
            this.groupBox8.Controls.Add(this.nud_ysize1);
            this.groupBox8.Controls.Add(this.label53);
            this.groupBox8.Controls.Add(this.nud_xoffset1);
            this.groupBox8.Controls.Add(this.nud_yoffset1);
            this.groupBox8.Location = new System.Drawing.Point(6, 173);
            this.groupBox8.Name = "groupBox8";
            this.groupBox8.Size = new System.Drawing.Size(296, 161);
            this.groupBox8.TabIndex = 7;
            this.groupBox8.TabStop = false;
            this.groupBox8.Text = "Profile 1";
            // 
            // label46
            // 
            this.label46.AutoSize = true;
            this.label46.Location = new System.Drawing.Point(17, 26);
            this.label46.Name = "label46";
            this.label46.Size = new System.Drawing.Size(61, 13);
            this.label46.TabIndex = 6;
            this.label46.Text = "Scan mode";
            // 
            // cb_vidscan1
            // 
            this.cb_vidscan1.FormattingEnabled = true;
            this.cb_vidscan1.Items.AddRange(new object[] {
            "Progressive",
            "Interlaced"});
            this.cb_vidscan1.Location = new System.Drawing.Point(154, 23);
            this.cb_vidscan1.Name = "cb_vidscan1";
            this.cb_vidscan1.Size = new System.Drawing.Size(121, 21);
            this.cb_vidscan1.TabIndex = 7;
            this.cb_vidscan1.Text = "Interlaced";
            this.cb_vidscan1.SelectedIndexChanged += new System.EventHandler(this.video1_config_changed);
            // 
            // cb_xsize1
            // 
            this.cb_xsize1.FormattingEnabled = true;
            this.cb_xsize1.Items.AddRange(new object[] {
            "420",
            "480",
            "560",
            "672"});
            this.cb_xsize1.Location = new System.Drawing.Point(154, 50);
            this.cb_xsize1.Name = "cb_xsize1";
            this.cb_xsize1.Size = new System.Drawing.Size(121, 21);
            this.cb_xsize1.TabIndex = 8;
            this.cb_xsize1.Text = "480";
            this.cb_xsize1.SelectedIndexChanged += new System.EventHandler(this.video1_config_changed);
            // 
            // label49
            // 
            this.label49.AutoSize = true;
            this.label49.Location = new System.Drawing.Point(18, 53);
            this.label49.Name = "label49";
            this.label49.Size = new System.Drawing.Size(35, 13);
            this.label49.TabIndex = 13;
            this.label49.Text = "X size";
            // 
            // label51
            // 
            this.label51.AutoSize = true;
            this.label51.Location = new System.Drawing.Point(17, 131);
            this.label51.Name = "label51";
            this.label51.Size = new System.Drawing.Size(78, 13);
            this.label51.TabIndex = 11;
            this.label51.Text = "Y bottom offset";
            // 
            // label52
            // 
            this.label52.AutoSize = true;
            this.label52.Location = new System.Drawing.Point(17, 79);
            this.label52.Name = "label52";
            this.label52.Size = new System.Drawing.Size(60, 13);
            this.label52.TabIndex = 12;
            this.label52.Text = "X left offset";
            // 
            // nud_ysize1
            // 
            this.nud_ysize1.Location = new System.Drawing.Point(153, 129);
            this.nud_ysize1.Maximum = new decimal(new int[] {
            200,
            0,
            0,
            0});
            this.nud_ysize1.Name = "nud_ysize1";
            this.nud_ysize1.Size = new System.Drawing.Size(120, 20);
            this.nud_ysize1.TabIndex = 18;
            this.nud_ysize1.ValueChanged += new System.EventHandler(this.video1_config_changed);
            // 
            // label53
            // 
            this.label53.AutoSize = true;
            this.label53.Location = new System.Drawing.Point(17, 105);
            this.label53.Name = "label53";
            this.label53.Size = new System.Drawing.Size(61, 13);
            this.label53.TabIndex = 10;
            this.label53.Text = "Y top offset";
            // 
            // nud_xoffset1
            // 
            this.nud_xoffset1.Location = new System.Drawing.Point(153, 77);
            this.nud_xoffset1.Maximum = new decimal(new int[] {
            400,
            0,
            0,
            0});
            this.nud_xoffset1.Name = "nud_xoffset1";
            this.nud_xoffset1.Size = new System.Drawing.Size(120, 20);
            this.nud_xoffset1.TabIndex = 14;
            this.nud_xoffset1.Value = new decimal(new int[] {
            120,
            0,
            0,
            0});
            this.nud_xoffset1.ValueChanged += new System.EventHandler(this.video1_config_changed);
            // 
            // nud_yoffset1
            // 
            this.nud_yoffset1.Location = new System.Drawing.Point(153, 103);
            this.nud_yoffset1.Maximum = new decimal(new int[] {
            400,
            0,
            0,
            0});
            this.nud_yoffset1.Minimum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.nud_yoffset1.Name = "nud_yoffset1";
            this.nud_yoffset1.Size = new System.Drawing.Size(120, 20);
            this.nud_yoffset1.TabIndex = 20;
            this.nud_yoffset1.Value = new decimal(new int[] {
            40,
            0,
            0,
            0});
            this.nud_yoffset1.ValueChanged += new System.EventHandler(this.video1_config_changed);
            // 
            // groupBox7
            // 
            this.groupBox7.Controls.Add(this.label1);
            this.groupBox7.Controls.Add(this.cb_vidscan);
            this.groupBox7.Controls.Add(this.cb_xsize);
            this.groupBox7.Controls.Add(this.label2);
            this.groupBox7.Controls.Add(this.label3);
            this.groupBox7.Controls.Add(this.label4);
            this.groupBox7.Controls.Add(this.label5);
            this.groupBox7.Controls.Add(this.nud_ysize);
            this.groupBox7.Controls.Add(this.nud_xoffset);
            this.groupBox7.Controls.Add(this.nud_yoffset);
            this.groupBox7.Location = new System.Drawing.Point(6, 6);
            this.groupBox7.Name = "groupBox7";
            this.groupBox7.Size = new System.Drawing.Size(296, 161);
            this.groupBox7.TabIndex = 6;
            this.groupBox7.TabStop = false;
            this.groupBox7.Text = "Profile 0 (default)";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(17, 26);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(61, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Scan mode";
            // 
            // cb_vidscan
            // 
            this.cb_vidscan.FormattingEnabled = true;
            this.cb_vidscan.Items.AddRange(new object[] {
            "Progressive",
            "Interlaced"});
            this.cb_vidscan.Location = new System.Drawing.Point(153, 23);
            this.cb_vidscan.Name = "cb_vidscan";
            this.cb_vidscan.Size = new System.Drawing.Size(121, 21);
            this.cb_vidscan.TabIndex = 1;
            this.cb_vidscan.Text = "Progressive";
            this.cb_vidscan.SelectedIndexChanged += new System.EventHandler(this.video0_config_changed);
            // 
            // cb_xsize
            // 
            this.cb_xsize.FormattingEnabled = true;
            this.cb_xsize.Items.AddRange(new object[] {
            "420",
            "480",
            "560",
            "672"});
            this.cb_xsize.Location = new System.Drawing.Point(153, 50);
            this.cb_xsize.Name = "cb_xsize";
            this.cb_xsize.Size = new System.Drawing.Size(121, 21);
            this.cb_xsize.TabIndex = 1;
            this.cb_xsize.Text = "480";
            this.cb_xsize.SelectedIndexChanged += new System.EventHandler(this.video0_config_changed);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(17, 53);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(35, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "X size";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(18, 131);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(78, 13);
            this.label3.TabIndex = 2;
            this.label3.Text = "Y bottom offset";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(17, 79);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(60, 13);
            this.label4.TabIndex = 2;
            this.label4.Text = "X left offset";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(18, 105);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(61, 13);
            this.label5.TabIndex = 2;
            this.label5.Text = "Y top offset";
            // 
            // nud_ysize
            // 
            this.nud_ysize.Location = new System.Drawing.Point(153, 129);
            this.nud_ysize.Maximum = new decimal(new int[] {
            200,
            0,
            0,
            0});
            this.nud_ysize.Name = "nud_ysize";
            this.nud_ysize.Size = new System.Drawing.Size(120, 20);
            this.nud_ysize.TabIndex = 3;
            this.nud_ysize.ValueChanged += new System.EventHandler(this.video0_config_changed);
            // 
            // nud_xoffset
            // 
            this.nud_xoffset.Location = new System.Drawing.Point(153, 77);
            this.nud_xoffset.Maximum = new decimal(new int[] {
            400,
            0,
            0,
            0});
            this.nud_xoffset.Name = "nud_xoffset";
            this.nud_xoffset.Size = new System.Drawing.Size(120, 20);
            this.nud_xoffset.TabIndex = 3;
            this.nud_xoffset.Value = new decimal(new int[] {
            120,
            0,
            0,
            0});
            this.nud_xoffset.ValueChanged += new System.EventHandler(this.video0_config_changed);
            // 
            // nud_yoffset
            // 
            this.nud_yoffset.Location = new System.Drawing.Point(153, 103);
            this.nud_yoffset.Maximum = new decimal(new int[] {
            400,
            0,
            0,
            0});
            this.nud_yoffset.Minimum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.nud_yoffset.Name = "nud_yoffset";
            this.nud_yoffset.Size = new System.Drawing.Size(120, 20);
            this.nud_yoffset.TabIndex = 3;
            this.nud_yoffset.Value = new decimal(new int[] {
            40,
            0,
            0,
            0});
            this.nud_yoffset.ValueChanged += new System.EventHandler(this.video0_config_changed);
            // 
            // tabControl1
            // 
            this.tabControl1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tabControl1.Controls.Add(this.tab_video);
            this.tabControl1.Controls.Add(this.tab_uarts);
            this.tabControl1.Controls.Add(this.tab_mavlink);
            this.tabControl1.Controls.Add(this.tab_alarms);
            this.tabControl1.Controls.Add(this.tab_misc);
            this.tabControl1.Controls.Add(this.tab_widgets);
            this.tabControl1.Controls.Add(this.tab_log);
            this.tabControl1.Location = new System.Drawing.Point(12, 27);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(1173, 503);
            this.tabControl1.TabIndex = 0;
            this.tabControl1.SelectedIndexChanged += new System.EventHandler(this.tabControl1_SelectedIndexChanged);
            // 
            // timer_submit_cfg
            // 
            this.timer_submit_cfg.Interval = 500;
            this.timer_submit_cfg.Tick += new System.EventHandler(this.timer_submit_cfg_Tick);
            // 
            // bt_flash_fw
            // 
            this.bt_flash_fw.Location = new System.Drawing.Point(774, 19);
            this.bt_flash_fw.Name = "bt_flash_fw";
            this.bt_flash_fw.Size = new System.Drawing.Size(96, 23);
            this.bt_flash_fw.TabIndex = 16;
            this.bt_flash_fw.Text = "Flash firmware...";
            this.bt_flash_fw.UseVisualStyleBackColor = true;
            this.bt_flash_fw.Click += new System.EventHandler(this.bt_flash_fw_Click);
            // 
            // AlceOSDconfigForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1197, 901);
            this.Controls.Add(this.gb_shell);
            this.Controls.Add(this.pb);
            this.Controls.Add(this.tabControl1);
            this.Controls.Add(this.menuStrip1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "AlceOSDconfigForm";
            this.Text = "AlceOSD 0v12";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form2_FormClosing);
            this.Load += new System.EventHandler(this.AlceOSDconfigForm_Load);
            ((System.ComponentModel.ISupportInitialize)(this.tb_tlog)).EndInit();
            this.cm_widget.ResumeLayout(false);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.gb_shell.ResumeLayout(false);
            this.gb_shell.PerformLayout();
            this.tab_log.ResumeLayout(false);
            this.tab_log.PerformLayout();
            this.tab_widgets.ResumeLayout(false);
            this.tab_widgets.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nud_wypos)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_wxpos)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pb_osd)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_seltab)).EndInit();
            this.tab_misc.ResumeLayout(false);
            this.gb_rssi.ResumeLayout(false);
            this.gb_rssi.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nud_rssi_max)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_rssi_min)).EndInit();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nud_tabmax)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_tabmin)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_tabtimer)).EndInit();
            this.tab_alarms.ResumeLayout(false);
            this.tab_alarms.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nud_fa_timer)).EndInit();
            this.tab_mavlink.ResumeLayout(false);
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nud_streamExtra3)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_streamExtra2)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_streamExtra1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_streamPosition)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_streamRawCtrl)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_streamRcChannels)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_streamExtStatus)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_streamRawSensors)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_osdsysid)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_uavsysid)).EndInit();
            this.tab_uarts.ResumeLayout(false);
            this.gb_uart4.ResumeLayout(false);
            this.gb_uart4.PerformLayout();
            this.gb_uart3.ResumeLayout(false);
            this.gb_uart3.PerformLayout();
            this.gb_uart2.ResumeLayout(false);
            this.gb_uart2.PerformLayout();
            this.gb_uart1.ResumeLayout(false);
            this.gb_uart1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pb_uarthw)).EndInit();
            this.tab_video.ResumeLayout(false);
            this.gb_vsync.ResumeLayout(false);
            this.gb_vsync.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nud_vsync1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_vsync0)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nud_whitelvl)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_graylvl)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_blacklvl)).EndInit();
            this.gb_vid0v4.ResumeLayout(false);
            this.gb_vid0v4.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nud_vswmax)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_vswmin)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_vswtimer)).EndInit();
            this.groupBox8.ResumeLayout(false);
            this.groupBox8.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nud_ysize1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_xoffset1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_yoffset1)).EndInit();
            this.groupBox7.ResumeLayout(false);
            this.groupBox7.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nud_ysize)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_xoffset)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_yoffset)).EndInit();
            this.tabControl1.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openConfigToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveConfigToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveAsToolStripMenuItem;
        private System.Windows.Forms.OpenFileDialog ofd_loadcfg;
        private System.Windows.Forms.ContextMenuStrip cm_widget;
        private System.Windows.Forms.ToolStripMenuItem moveToTabToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem addWidgetToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem removeToolStripMenuItem;
        private System.Windows.Forms.SaveFileDialog ofd_savecfg;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem boardToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem readConfigToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem writeConfigToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.OpenFileDialog ofd_fwfile;
        private System.Windows.Forms.ProgressBar pb;
        private System.Windows.Forms.ToolStripMenuItem downloadFirmwareToolStripMenuItem;
        private System.Windows.Forms.TextBox txt_shell;
        private System.Windows.Forms.TextBox tb_cmdLine;
        private System.Windows.Forms.Button bt_conn;
        private System.Windows.Forms.Timer timer_com;
        private System.Windows.Forms.ListBox lb_history;
        private System.Windows.Forms.Timer timer_submit;
        private System.Windows.Forms.Button bt_submitCfg;
        private System.Windows.Forms.Label label60;
        private System.Windows.Forms.CheckBox cbx_mavmode;
        private System.Windows.Forms.Timer timer_heartbeat;
        private System.Windows.Forms.Button bt_reboot;
        private System.Windows.Forms.Button bt_sendTlog;
        private System.Windows.Forms.OpenFileDialog ofd_tlog;
        private System.Windows.Forms.TrackBar tb_tlog;
        private System.Windows.Forms.Button bt_upCfg;
        private System.Windows.Forms.OpenFileDialog ofd_upCfg;
        private System.Windows.Forms.Button bt_dnCfg;
        private System.Windows.Forms.SaveFileDialog ofd_dnCfg;
        private System.Windows.Forms.GroupBox gb_shell;
        private System.Windows.Forms.ComboBox cb_comport;
        private System.Windows.Forms.TabPage tab_log;
        private System.Windows.Forms.TextBox txt_log;
        private System.Windows.Forms.TabPage tab_widgets;
        private System.Windows.Forms.ComboBox cb_units;
        private System.Windows.Forms.Label label32;
        private System.Windows.Forms.Button bt_refreshCanvas;
        private System.Windows.Forms.TextBox tb_wp4;
        private System.Windows.Forms.TextBox tb_wp3;
        private System.Windows.Forms.TextBox tb_wp2;
        private System.Windows.Forms.TextBox tb_wp1;
        private System.Windows.Forms.ComboBox cb_wsource;
        private System.Windows.Forms.ComboBox cb_wunits;
        private System.Windows.Forms.ComboBox cb_wmode;
        private System.Windows.Forms.ComboBox cb_whjust;
        private System.Windows.Forms.ComboBox cb_wvjust;
        private System.Windows.Forms.NumericUpDown nud_wypos;
        private System.Windows.Forms.NumericUpDown nud_wxpos;
        private System.Windows.Forms.Label lbl_wp4;
        private System.Windows.Forms.Label lbl_wp3;
        private System.Windows.Forms.Label lbl_wp2;
        private System.Windows.Forms.Label lbl_wp1;
        private System.Windows.Forms.Label lbl_wsource;
        private System.Windows.Forms.Label label20;
        private System.Windows.Forms.Label lbl_wmode;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.Label lbl_wname;
        private System.Windows.Forms.PictureBox pb_osd;
        private System.Windows.Forms.ListBox lb_widgets;
        private System.Windows.Forms.NumericUpDown nud_seltab;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.TabPage tab_misc;
        private System.Windows.Forms.GroupBox gb_rssi;
        private System.Windows.Forms.NumericUpDown nud_rssi_max;
        private System.Windows.Forms.NumericUpDown nud_rssi_min;
        private System.Windows.Forms.ComboBox cb_rssi_units;
        private System.Windows.Forms.ComboBox cb_rssi_src;
        private System.Windows.Forms.Label label63;
        private System.Windows.Forms.Label label62;
        private System.Windows.Forms.Label label61;
        private System.Windows.Forms.Label label31;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.NumericUpDown nud_tabmax;
        private System.Windows.Forms.NumericUpDown nud_tabmin;
        private System.Windows.Forms.Label label30;
        private System.Windows.Forms.Label label29;
        private System.Windows.Forms.ComboBox cb_tabch;
        private System.Windows.Forms.NumericUpDown nud_tabtimer;
        private System.Windows.Forms.Label label28;
        private System.Windows.Forms.Label label27;
        private System.Windows.Forms.ComboBox cb_tabmode;
        private System.Windows.Forms.Label label26;
        private System.Windows.Forms.TabPage tab_alarms;
        private System.Windows.Forms.Button bt_fa_savealarm;
        private System.Windows.Forms.Button bt_fa_del;
        private System.Windows.Forms.Button bt_fa_add;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.TextBox tb_fa_value;
        private System.Windows.Forms.Label lbl_fa_name;
        private System.Windows.Forms.Label label54;
        private System.Windows.Forms.Label label50;
        private System.Windows.Forms.NumericUpDown nud_fa_timer;
        private System.Windows.Forms.ComboBox cb_fa_type;
        private System.Windows.Forms.Label label48;
        private System.Windows.Forms.Label label47;
        private System.Windows.Forms.ListBox lb_fa_cfg;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.ListBox lb_fa;
        private System.Windows.Forms.TabPage tab_mavlink;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.CheckBox cbx_mavhb;
        private System.Windows.Forms.Label label45;
        private System.Windows.Forms.NumericUpDown nud_streamExtra3;
        private System.Windows.Forms.Label label44;
        private System.Windows.Forms.NumericUpDown nud_streamExtra2;
        private System.Windows.Forms.Label label43;
        private System.Windows.Forms.NumericUpDown nud_streamExtra1;
        private System.Windows.Forms.Label label42;
        private System.Windows.Forms.NumericUpDown nud_streamPosition;
        private System.Windows.Forms.Label label41;
        private System.Windows.Forms.NumericUpDown nud_streamRawCtrl;
        private System.Windows.Forms.Label label40;
        private System.Windows.Forms.NumericUpDown nud_streamRcChannels;
        private System.Windows.Forms.Label label39;
        private System.Windows.Forms.NumericUpDown nud_streamExtStatus;
        private System.Windows.Forms.Label label38;
        private System.Windows.Forms.NumericUpDown nud_streamRawSensors;
        private System.Windows.Forms.Label label37;
        private System.Windows.Forms.NumericUpDown nud_osdsysid;
        private System.Windows.Forms.NumericUpDown nud_uavsysid;
        private System.Windows.Forms.Label label34;
        private System.Windows.Forms.Label label33;
        private System.Windows.Forms.TabPage tab_uarts;
        private System.Windows.Forms.GroupBox gb_uart4;
        private System.Windows.Forms.Label label23;
        private System.Windows.Forms.Label label24;
        private System.Windows.Forms.ComboBox cb_baud4;
        private System.Windows.Forms.ComboBox cb_mode4;
        private System.Windows.Forms.GroupBox gb_uart3;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.Label label21;
        private System.Windows.Forms.ComboBox cb_baud3;
        private System.Windows.Forms.ComboBox cb_mode3;
        private System.Windows.Forms.GroupBox gb_uart2;
        private System.Windows.Forms.ComboBox cb_baud2;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.ComboBox cb_mode2;
        private System.Windows.Forms.GroupBox gb_uart1;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.ComboBox cb_baud1;
        private System.Windows.Forms.ComboBox cb_mode1;
        private System.Windows.Forms.PictureBox pb_uarthw;
        private System.Windows.Forms.TabPage tab_video;
        private System.Windows.Forms.GroupBox gb_vsync;
        private System.Windows.Forms.Label label66;
        private System.Windows.Forms.NumericUpDown nud_vsync1;
        private System.Windows.Forms.Label label65;
        private System.Windows.Forms.NumericUpDown nud_vsync0;
        private System.Windows.Forms.Label label64;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.NumericUpDown nud_whitelvl;
        private System.Windows.Forms.Label label36;
        private System.Windows.Forms.NumericUpDown nud_graylvl;
        private System.Windows.Forms.Label label35;
        private System.Windows.Forms.NumericUpDown nud_blacklvl;
        private System.Windows.Forms.Label label25;
        private System.Windows.Forms.GroupBox gb_vid0v4;
        private System.Windows.Forms.NumericUpDown nud_vswmax;
        private System.Windows.Forms.NumericUpDown nud_vswmin;
        private System.Windows.Forms.Label label55;
        private System.Windows.Forms.Label label56;
        private System.Windows.Forms.ComboBox cb_vswch;
        private System.Windows.Forms.NumericUpDown nud_vswtimer;
        private System.Windows.Forms.Label label57;
        private System.Windows.Forms.Label label58;
        private System.Windows.Forms.ComboBox cb_vswmode;
        private System.Windows.Forms.Label label59;
        private System.Windows.Forms.GroupBox groupBox8;
        private System.Windows.Forms.Label label46;
        private System.Windows.Forms.ComboBox cb_vidscan1;
        private System.Windows.Forms.ComboBox cb_xsize1;
        private System.Windows.Forms.Label label49;
        private System.Windows.Forms.Label label51;
        private System.Windows.Forms.Label label52;
        private System.Windows.Forms.NumericUpDown nud_ysize1;
        private System.Windows.Forms.Label label53;
        private System.Windows.Forms.NumericUpDown nud_xoffset1;
        private System.Windows.Forms.NumericUpDown nud_yoffset1;
        private System.Windows.Forms.GroupBox groupBox7;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox cb_vidscan;
        private System.Windows.Forms.ComboBox cb_xsize;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.NumericUpDown nud_ysize;
        private System.Windows.Forms.NumericUpDown nud_xoffset;
        private System.Windows.Forms.NumericUpDown nud_yoffset;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.ComboBox cb_hwrev;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.ToolStripMenuItem importOldConfigToolStripMenuItem;
        private System.Windows.Forms.Timer timer_submit_cfg;
        private System.Windows.Forms.Button bt_flash_fw;
    }
}