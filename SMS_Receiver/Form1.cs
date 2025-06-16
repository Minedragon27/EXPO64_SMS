using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using SMS_Receiver;
using System.Runtime.InteropServices;
using System.IO.Ports;
using System.Threading;
using System.IO;
using System.Diagnostics;



namespace SMS_Receiver
{
    public partial class Form1 : Form
    {
        static SerialPort serialPort;
        /* TODO
        _serialPort.Parity = SetPortParity(_serialPort.Parity);
        _serialPort.DataBits = SetPortDataBits(_serialPort.DataBits);
        _serialPort.StopBits = SetPortStopBits(_serialPort.StopBits);
        _serialPort.Handshake = SetPortHandshake(_serialPort.Handshake);
        */
        static RadioButton[] radioButtonsPorts;
        Thread readThread;
        Thread writeThread;
        bool enableReading = false;
        bool continueWriting = false;
        List<float[]> dataLogCache=new List<float[]>();
        public Form1()
        {
            InitializeComponent();
        }
        private void scanPorts()
        {

            string[] ports = SerialPort.GetPortNames();
            foreach (RadioButton button in radioButtonsPorts)
            {
                button.Enabled = false;
                button.Visible = false;
            }
           
            for (int i = 0; i <radioButtonsPorts.Length && i < ports.Length; i++)
            {
                radioButtonsPorts[i].Enabled = true;
                radioButtonsPorts[i].Visible = true;
                radioButtonsPorts[i].Text = ports[i];
            }
        }
        private void Form1_Load(object sender, EventArgs e)
        {
            radioButtonsPorts = new RadioButton[] { radioButton1, radioButton2, radioButton3, radioButton4, radioButton5 };
            serialPort = new SerialPort();
            comboBoxBaudRate.SelectedIndex = 4;

            
        }
        
        

        private void buttonShowPorts_Click(object sender, EventArgs e)
        {
            scanPorts();
            buttonConnect.Enabled = true;
        }

        private void buttonConnect_Click(object sender, EventArgs e)
        {
            buttonDisconnect_Click(sender, e);
            string selectedPort="";
            foreach (RadioButton button in radioButtonsPorts)
            {
                button.Enabled = false;//disable all buttons
                if (button.Checked) selectedPort=button.Text;//check which is pressed
            }
            if (selectedPort == "")
            {
                foreach (RadioButton button in radioButtonsPorts)
                {
                    button.Enabled = true;
                }
                return;//return if no port is selected
            }

            serialPort.PortName = selectedPort;

            //label2.Text = comboBoxBaudRate.Text;
            int baudRate=int.Parse(comboBoxBaudRate.Text);
            
            serialPort.BaudRate = baudRate;
            serialPort.ReadTimeout = 500;
            serialPort.WriteTimeout = 500;
            serialPort.NewLine = "\r\n";
            serialPort.DtrEnable = true;
            serialPort.RtsEnable = true;

            comboBoxBaudRate.Enabled = false;

            buttonConnect.Enabled = false;//disable selecting new buttons until a rescan
            buttonDisconnect.Enabled = true;
            enableReading = true;



            try
            {

                serialPort.Open();
                buttonSendRequest.Enabled = true;
            }
            catch (UnauthorizedAccessException)
            {
                MessageBox.Show("Port already in use");
                buttonDisconnect_Click(sender, e);

            }
            catch (System.IO.IOException)
            {
                MessageBox.Show("Port no longer exists");
                scanPorts();
                buttonDisconnect_Click(sender, e);
            }
            

        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            serialPort.Close();
            buttonDisconnect_Click(sender,e);
        }



        private void buttonDisconnect_Click(object sender, EventArgs e)
        {
            enableReading = false;
            continueWriting = false;

            // Wait for threads to stop gracefully
            if (readThread != null && readThread.IsAlive)
                readThread.Join();
            if (writeThread != null && writeThread.IsAlive)
                writeThread.Join();

            if (serialPort.IsOpen)
                serialPort.Close();

            comboBoxBaudRate.Enabled = true;
            buttonConnect.Enabled = true;
            buttonSendRequest.Enabled = false;
        }

        public void Read()
        {
            while (enableReading&&serialPort.IsOpen)
            {
                try
                {
                    if (serialPort.BytesToRead == 0) continue;
                    
                    string message = serialPort.ReadLine();
                    labelStatus.Invoke((MethodInvoker)(() => labelStatus.Text = "Received a Message"));
                    labelStatus.Invoke((MethodInvoker)(() => labelStatus.ForeColor = Color.Blue));
                    if (message == "transmitting")
                    {
                        continueWriting = false;//stop requesting data

                        List<float[]> receivedData = new List<float[]>();
                        labelStatus.Invoke((MethodInvoker)(() => labelStatus.Text = "Receiving Data"));
                        labelStatus.Invoke((MethodInvoker)(() => labelStatus.ForeColor = Color.GreenYellow));
                        while (message != "end_transmission")//reads all lines until end of transmission
                        {
                            float[] currLine = new float[5];
                            for (int i = 0; i < 5; i++) //reads each of the 5 values
                            {
                                
                                while (serialPort.BytesToRead == 0) ;
                                message = serialPort.ReadLine();
                                if (message == "end_transmission") break;
                                currLine[i] = float.Parse(message.Split(' ')[1]);
                                //adds every other value to the array
                            }//do for each row
                            if (message == "end_transmission") break;
                            while (serialPort.BytesToRead == 0) ;
                            message =serialPort.ReadLine();//skip separator row
                            receivedData.Add(currLine);//appends array to list
                            buttonDataOutput.Invoke((MethodInvoker)(() => buttonDataOutput.Enabled = true)); // disable


                        }
                        labelStatus.Invoke((MethodInvoker)(() => labelStatus.Text = "Data received"));
                        labelStatus.Invoke((MethodInvoker)(() => labelStatus.ForeColor = Color.Green));
                        enableReading = false;
                        continueWriting = false;
                        dataLogCache = receivedData;

                    }
                }
                catch (TimeoutException) 
                {
                    labelStatus.Invoke((MethodInvoker)(() => labelStatus.Text = "Error"));
                    labelStatus.Invoke((MethodInvoker)(() => labelStatus.ForeColor = Color.Red));
                    MessageBox.Show("Timeout error");
                }
            }
        }
        public void Write()
        {
            
        }
        private void buttonSendRequest_Click(object sender, EventArgs e)
        {
             
            continueWriting = true;
            if (readThread==null)
            {
                readThread = new Thread(Read);
                if(!readThread.IsAlive)readThread.Start();
            }

            buttonDataOutput.Enabled = true;
            try
            {
                serialPort.WriteLine("p");
            }
            catch (Exception ex) { MessageBox.Show(ex.ToString()); }
        }

        private void buttonDataOutput_Click(object sender, EventArgs e)
        {
            labelStatus.Text = "Idle";
            labelStatus.ForeColor = Color.Black;
            saveFileDialogDataLogOutput.Filter = "CSV files (*.csv)|*.csv|All files (*.*)|*.*";
            string path;
            if (saveFileDialogDataLogOutput.ShowDialog() == DialogResult.OK)
            {
                path = saveFileDialogDataLogOutput.FileName;//get path
                

            }
            else return;//return if not ok
            try
            {
                using (StreamWriter writer = new StreamWriter(path))
                {
                    writer.WriteLine("time,temperature,humidity,co2,light");
                    Console.WriteLine("Rows of data: "+dataLogCache.Count);
                    foreach (float[] row in dataLogCache)
                    {
                        
                        string line = string.Join(",", row.Select(f => f.ToString(System.Globalization.CultureInfo.InvariantCulture)));
                        Console.WriteLine(line);
                        writer.WriteLine(line);
                    }
                }
                dataLogCache = new List<float[]>();
                buttonDataOutput.Enabled = false;
            }
            catch (IOException)
            {
                MessageBox.Show("Cannot edit file; file already in use");
            }
            
        }


    }
}
