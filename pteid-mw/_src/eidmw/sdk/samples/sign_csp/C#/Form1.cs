using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

using be.belgium.eid;

using DWORD = System.UInt32;

using HCRYPTPROV = System.UInt32;
using HCRYPTKEY = System.UInt32;
using HCRYPTHASH = System.UInt32;
using ALG_ID = System.UInt32;

namespace sign_csp
{
    public partial class Form1 : Form
    {
        const uint PROV_RSA_FULL = 1;
        const uint AT_SIGNATURE = 2;

        const uint CRYPT_VERIFYCONTEXT   = 0xF0000000;

        const uint CALG_SHA1  = 0x00008004;

        [DllImport(@"advapi32.dll", EntryPoint = "CryptAcquireContextA",CharSet = CharSet.Ansi, SetLastError = true)]
        internal static extern bool CryptAcquireContext(out HCRYPTPROV phProv, string pszContainer, string pszProvider, DWORD dwProvType, DWORD dwFlags);

        [DllImport(@"advapi32.dll", EntryPoint = "CryptReleaseContext",CharSet = CharSet.Ansi, SetLastError = true)]
        internal static extern bool CryptReleaseContext(HCRYPTPROV hProv, DWORD dwFlags);

        [DllImport(@"advapi32.dll", EntryPoint = "CryptCreateHash",CharSet = CharSet.Ansi, SetLastError = true)]
        internal static extern bool CryptCreateHash(HCRYPTPROV hProv, ALG_ID Algid, HCRYPTKEY hKey, DWORD dwFlags, out HCRYPTHASH phHash);

        [DllImport(@"advapi32.dll", EntryPoint = "CryptDestroyHash",CharSet = CharSet.Ansi, SetLastError = true)]
        internal static extern bool CryptDestroyHash(HCRYPTHASH hHash);

        [DllImport(@"advapi32.dll", EntryPoint = "CryptHashData",CharSet = CharSet.Ansi, SetLastError = true)]
        internal static extern bool CryptHashData(HCRYPTHASH hHash, byte[] pbData, DWORD dwDataLen, DWORD dwFlags);

        [DllImport(@"advapi32.dll", EntryPoint = "CryptSignHashA",CharSet = CharSet.Ansi, SetLastError = true)]
        internal static extern bool CryptSignHash(HCRYPTHASH hHash, DWORD dwKeySpec, string sDescription, DWORD dwFlags, IntPtr pbSignature, ref DWORD pdwSigLen);

        public Form1()
        {
            InitializeComponent();
        }

        private void btnQuit_Click(object sender, EventArgs e)
        {
            this.Close();

            BEID_ReaderSet.releaseSDK();

            Application.Exit();
        }

        private void btnTest_Click(object sender, EventArgs e)
        {
            //Get the name of the container
            BEID_ReaderContext reader = BEID_ReaderSet.instance().getReader();
            BEID_EIDCard card = reader.getEIDCard();
            BEID_CardVersionInfo doc = card.getVersionInfo();

            string container = "Signature(" + doc.getSerialNumber() + ")";
            string provider = "Belgium Identity Card CSP";

            HCRYPTPROV hprov;

            //Get a context
            if (!CryptAcquireContext(out hprov, container, provider, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
            {
                MessageBox.Show("CryptAcquireContext failed.\n\n(Error=" + Marshal.GetLastWin32Error().ToString("x") + ")"); 
                return;
            }
            	
            //Open the data to sign
            byte[] data;
            uint data_len=0;

            string fileToSign = "sign_csp.exe";

            if (!System.IO.File.Exists(fileToSign))															
            {
                MessageBox.Show("Could not find file " + fileToSign ); 
               return;
            }
            data = System.IO.File.ReadAllBytes(fileToSign);
            data_len = (uint)data.GetLength(0);

            HCRYPTHASH hhash;

            //Get an handle to a new hash
            if (!CryptCreateHash(hprov, CALG_SHA1, 0, 0, out hhash))
            {
                MessageBox.Show("CryptCreateHash failed.\n\n(Error=" + Marshal.GetLastWin32Error().ToString("x") + ")");
                return;
            }

            //Create the hash
            if (!CryptHashData(hhash, data, data_len, 0))
            {
                MessageBox.Show("CryptHashData failed.\n\n(Error=" + Marshal.GetLastWin32Error().ToString("x") + ")");
                return;
            }

            uint sig_len = 1024;
            IntPtr ptSignature = Marshal.AllocHGlobal((int)sig_len);

            //Sign the hash
            if (!CryptSignHash(hhash, AT_SIGNATURE, "", 0, ptSignature, ref sig_len))
            {
                MessageBox.Show("CryptSignHash failed.\n\n(Error=" + Marshal.GetLastWin32Error().ToString("x") + ")");
                return;
            }
            byte[] signature = new byte[sig_len];
            Marshal.Copy(ptSignature, signature, 0, (int)sig_len);

            //Write the signature into a file
            System.IO.File.WriteAllBytes("sign_csp_cs.sig", signature);

            Marshal.FreeHGlobal(ptSignature);

            //Release hash
            if (!CryptDestroyHash(hhash))
            {
                MessageBox.Show("CryptDestroyHash failed.\n\n(Error=" + Marshal.GetLastWin32Error().ToString("x") + ")");
                 return;
            }

            //Release context
            if (!CryptReleaseContext(hprov, 0))
            {
                MessageBox.Show("CryptReleaseContext failed.\n\n(Error=" + Marshal.GetLastWin32Error().ToString("x") + ")");
                return;
            }

            MessageBox.Show("Signing successful");
        }
    }
}