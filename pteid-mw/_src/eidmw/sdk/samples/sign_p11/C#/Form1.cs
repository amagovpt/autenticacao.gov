using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

using CK_ULONG = System.UInt32;
using CK_VOID_PTR = System.IntPtr;
using CK_BBOOL = System.Boolean;
using CK_SLOT_ID = System.UInt32;           //CK_ULONG;
using CK_OBJECT_CLASS = System.UInt32;      //CK_ULONG;
using CK_SESSION_HANDLE = System.UInt32;    //CK_ULONG;
using CK_FLAGS = System.UInt32;             //CK_ULONG;
using CK_NOTIFY = System.IntPtr;            //Callback function, not used in this sample
using CK_OBJECT_HANDLE = System.UInt32;     //CK_ULONG;
using CK_ATTRIBUTE_TYPE = System.UInt32;    //CK_ULONG;
using CK_MECHANISM_TYPE = System.UInt32;    //CK_ULONG;
using CK_BYTE = System.Byte;

namespace sign_p11
{
    public unsafe class Memory //Code from http://msdn.microsoft.com/en-us/library/aa664786(VS.71).aspx
    {
        // Handle for the process heap. This handle is used in all calls to the
        // HeapXXX APIs in the methods below.
        static int ph = GetProcessHeap();
        // Private instance constructor to prevent instantiation.
        private Memory() {}
        // Allocates a memory block of the given size. The allocated memory is
        // automatically initialized to zero.
        public static void* Alloc(int size) {
            void* result = HeapAlloc(ph, HEAP_ZERO_MEMORY, size);
            if (result == null) throw new OutOfMemoryException();
            return result;
        }
        // Copies count bytes from src to dst. The source and destination
        // blocks are permitted to overlap.
        public static void Copy(void* src, void* dst, int count) {
            byte* ps = (byte*)src;
            byte* pd = (byte*)dst;
            if (ps > pd) {
                for (; count != 0; count--) *pd++ = *ps++;
            }
            else if (ps < pd) {
                for (ps += count, pd += count; count != 0; count--) *--pd = *--ps;
            }
        }
        // Copies count bytes from src to dst. 
        // and increment the dst pointer
        public static void Append(void* src, void* dst, int count)
        {
            Copy(src, dst, count);
            dst = (void*)((int)dst + count);
        }
        public static void Append(uint srcIn, ref void* dst)
        {
            int count = sizeof(uint);
            void* src = &srcIn;
            Copy(src, dst, count);
            dst = (void*)((int)dst + count);
        }
        public static void Append(uint* srcIn, ref void* dst)
        {
            int count = sizeof(uint*);
            void* src = &srcIn;
            Copy(src, dst, count);
            dst = (void*)((int)dst + count);
        }
        // Frees a memory block.
        public static void Free(void* block) {
            if (!HeapFree(ph, 0, block)) throw new InvalidOperationException();
        }
        // Re-allocates a memory block. If the reallocation request is for a
        // larger size, the additional region of memory is automatically
        // initialized to zero.
        public static void* ReAlloc(void* block, int size) {
            void* result = HeapReAlloc(ph, HEAP_ZERO_MEMORY, block, size);
            if (result == null) throw new OutOfMemoryException();
            return result;
        }
        // Returns the size of a memory block.
        public static int SizeOf(void* block) {
            int result = HeapSize(ph, 0, block);
            if (result == -1) throw new InvalidOperationException();
            return result;
        }
        // Heap API flags
        const int HEAP_ZERO_MEMORY = 0x00000008;
        // Heap API functions
        [DllImport("kernel32")]
        static extern int GetProcessHeap();
        [DllImport("kernel32")]
        static extern void* HeapAlloc(int hHeap, int flags, int size);
        [DllImport("kernel32")]
        static extern bool HeapFree(int hHeap, int flags, void* block);
        [DllImport("kernel32")]
        static extern void* HeapReAlloc(int hHeap, int flags,void* block, int size);
        [DllImport("kernel32")]
        static extern int HeapSize(int hHeap, int flags, void* block);
    }

    public partial class Form1 : Form
    {

        const string p11Library = "beidpkcs11.dll";

        const uint CKR_OK = 0;

        const uint CK_INVALID_HANDLE=0;

        const uint CKF_RW_SESSION = 2;
        const uint CKF_SERIAL_SESSION = 4;

        const uint CKO_PRIVATE_KEY = 3;

        const uint CKA_CLASS = 0;
        const uint CKA_ID = 0x00000102;

        const uint CKM_SHA1_RSA_PKCS = 0x00000006;

        [StructLayout(LayoutKind.Sequential)]
        unsafe struct CK_ATTRIBUTE
        {
            public CK_ATTRIBUTE_TYPE type;
            public void* pValue;
            public CK_ULONG ulValueLen;
        }

        [StructLayout(LayoutKind.Sequential)]
        struct CK_MECHANISM {
            public CK_MECHANISM_TYPE mechanism;
            public CK_VOID_PTR pParameter;          //This parameter is not used here, so unsafe struct is not needed
            public CK_ULONG ulParameterLen;
        }

        [DllImport(p11Library, EntryPoint = "C_Initialize", CharSet = CharSet.Ansi, SetLastError = true)]
        static extern uint C_Initialize(CK_VOID_PTR param);

        [DllImport(p11Library, EntryPoint = "C_Finalize", CharSet = CharSet.Ansi, SetLastError = true)]
        static extern uint C_Finalize(CK_VOID_PTR reserved);

        [DllImport(p11Library, EntryPoint = "C_GetSlotList", CharSet = CharSet.Ansi, SetLastError = true)]
        static extern uint C_GetSlotList(CK_BBOOL tokenPresent, ref CK_SLOT_ID pSlotList, ref CK_ULONG pulCount);

        [DllImport(p11Library, EntryPoint = "C_OpenSession", CharSet = CharSet.Ansi, SetLastError = true)]
        static extern uint C_OpenSession(CK_SLOT_ID slotID, CK_FLAGS flags, CK_VOID_PTR pApplication, CK_NOTIFY Notify, out CK_SESSION_HANDLE phSession);

        [DllImport(p11Library, EntryPoint = "C_CloseSession", CharSet = CharSet.Ansi, SetLastError = true)]
        static extern uint C_CloseSession(CK_SESSION_HANDLE hSession);

        [DllImport(p11Library, EntryPoint = "C_FindObjectsInit", CharSet = CharSet.Ansi, SetLastError = true)]
        unsafe static extern uint C_FindObjectsInit(CK_SESSION_HANDLE hSession, CK_ATTRIBUTE* pTemplate, CK_ULONG ulCount);

        [DllImport(p11Library, EntryPoint = "C_FindObjects", CharSet = CharSet.Ansi, SetLastError = true)]
        static extern uint C_FindObjects(CK_SESSION_HANDLE hSession, out CK_OBJECT_HANDLE phObject, CK_ULONG ulMaxObjectCount, ref CK_ULONG pulObjectCount);

        [DllImport(p11Library, EntryPoint = "C_FindObjectsFinal", CharSet = CharSet.Ansi, SetLastError = true)]
        static extern uint C_FindObjectsFinal(CK_SESSION_HANDLE hSession);

        [DllImport(p11Library, EntryPoint = "C_SignInit", CharSet = CharSet.Ansi, SetLastError = true)]
        static extern uint C_SignInit(CK_SESSION_HANDLE hSession,ref CK_MECHANISM  pMechanism, CK_OBJECT_HANDLE hKey);

        [DllImport(p11Library, EntryPoint = "C_Sign", CharSet = CharSet.Ansi, SetLastError = true)]
        static extern uint C_Sign(CK_SESSION_HANDLE hSession, CK_BYTE[] pData, CK_ULONG ulDataLen, IntPtr pSignature, ref CK_ULONG pulSignatureLen);
      
        public Form1()
        {
            InitializeComponent();
        }

        private void btnQuit_Click(object sender, EventArgs e)
        {
            this.Close();
            Application.Exit();
        }

        private void btnTest_Click(object sender, EventArgs e)
        {
            uint LastRV = CKR_OK;

            //Initialize PKCS#11
            LastRV = C_Initialize(CK_VOID_PTR.Zero);
            if (LastRV != CKR_OK) { MessageBox.Show("C_Initialize failed.\n\n(Error=" + LastRV.ToString("x") + ")"); return; }

            //Get the list of reader with card (we are only interested with the first one)
            CK_SLOT_ID p11_slot=0;
            CK_ULONG p11_num_slots=1;
            LastRV = C_GetSlotList(true, ref p11_slot, ref p11_num_slots);
            if (LastRV != CKR_OK) { MessageBox.Show("C_GetSlotList failed.\n\n(Error=" + LastRV.ToString("x") + ")"); return; }

            //Open the P11 session
            CK_SESSION_HANDLE p11_session = CK_INVALID_HANDLE;
            LastRV = C_OpenSession(p11_slot, CKF_SERIAL_SESSION | CKF_RW_SESSION, CK_VOID_PTR.Zero, CK_VOID_PTR.Zero, out p11_session);
            if (LastRV != CKR_OK) { MessageBox.Show("C_OpenSession failed.\n\n(Error=" + LastRV.ToString("x") + ")"); return; }

            //Find the signature private key
            unsafe
            {
                try
                {
                    CK_OBJECT_CLASS cls = CKO_PRIVATE_KEY;
                    CK_ULONG id = 3;
                    
                    int struct_size = 2 * sizeof(CK_ATTRIBUTE);
                    void* pAttrs = Memory.Alloc(struct_size);
                    void* pAppend = pAttrs;

                    Memory.Append(CKA_CLASS, ref pAppend);               //attrs[0].type
                    Memory.Append(&cls, ref pAppend);                    //attrs[0].pValue
                    Memory.Append(sizeof(CK_OBJECT_CLASS), ref pAppend); //attrs[0].ulValueLen

                    Memory.Append(CKA_ID, ref pAppend);                  //attrs[1].type
                    Memory.Append(&id, ref pAppend);                     //attrs[1].pValue
                    Memory.Append(sizeof(CK_ULONG), ref pAppend);        //attrs[1].ulValueLen

                    LastRV = C_FindObjectsInit(p11_session, (CK_ATTRIBUTE*)pAttrs, 2);
                    Memory.Free(pAttrs);
                }
                catch(OutOfMemoryException ex)
                { MessageBox.Show("Out of memory."); return; }
                catch (InvalidOperationException ex)
                { MessageBox.Show("Invalid memory operation."); return; }
                catch
                { MessageBox.Show("Invalid memory operation."); return; }
            }
            if (LastRV != CKR_OK) { MessageBox.Show("C_FindObjectsInit failed.\n\n(Error=" + LastRV.ToString("x") + ")"); return; }
            
            CK_OBJECT_HANDLE signaturekey=CK_INVALID_HANDLE;
            CK_ULONG count=0;
            LastRV=C_FindObjects(p11_session, out signaturekey, 1, ref count);
            if (LastRV != CKR_OK) { MessageBox.Show("C_FindObjects failed.\n\n(Error=" + LastRV.ToString("x") + ")"); return; }
            
            if(count==0)	 { MessageBox.Show("Signature key not found."); return; }
            
            LastRV=C_FindObjectsFinal(p11_session);
            if (LastRV != CKR_OK) { MessageBox.Show("C_FindObjectsFinal failed.\n\n(Error=" + LastRV.ToString("x") + ")"); return; }

            //Open the data to sign
            byte[] data;
            uint data_len = 0;

            string fileToSign = "sign_P11.exe";

            if (!System.IO.File.Exists(fileToSign))
            {
                MessageBox.Show("Could not find file " + fileToSign);
                return;
            }

            //Initialize the signature
            CK_MECHANISM mech;
            mech.mechanism = CKM_SHA1_RSA_PKCS;
            mech.pParameter = IntPtr.Zero;
            mech.ulParameterLen = 0;
            LastRV = C_SignInit(p11_session, ref mech, signaturekey);
            if (LastRV != CKR_OK) { MessageBox.Show("C_SignInit failed.\n\n(Error=" + LastRV.ToString("x") + ")"); return; }
            
            data = System.IO.File.ReadAllBytes(fileToSign);
            data_len = (uint)data.GetLength(0);

            //Sign the data
            uint sig_len = 1024;
            IntPtr ptSignature = Marshal.AllocHGlobal((int)sig_len);

            LastRV = C_Sign(p11_session, data, data_len, ptSignature, ref sig_len);
            if (LastRV != CKR_OK) { MessageBox.Show("C_Sign failed.\n\n(Error=" + LastRV.ToString("x") + ")"); return; }

            byte[] signature = new byte[sig_len];
            Marshal.Copy(ptSignature, signature, 0, (int)sig_len);

            //Write the signature into a file
            System.IO.File.WriteAllBytes("sign_p11_cs.sig", signature);

            Marshal.FreeHGlobal(ptSignature);

            //Close the session
            LastRV = C_CloseSession(p11_session);
            if (LastRV != CKR_OK) { MessageBox.Show("C_CloseSession failed.\n\n(Error=" + LastRV.ToString("x") + ")"); return; }
            
            //Finalize PKCS#11
            LastRV = C_Finalize(CK_VOID_PTR.Zero);
            if (LastRV != CKR_OK) { MessageBox.Show("C_Finalize failed.\n\n(Error=" + LastRV.ToString("x") + ")"); return; }
            
            
            MessageBox.Show("Signing successful");

        }
    }
}