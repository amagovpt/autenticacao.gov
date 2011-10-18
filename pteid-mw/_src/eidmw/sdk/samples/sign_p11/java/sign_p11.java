import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.File;
import java.io.FileInputStream;
import java.io.DataInputStream;
import java.io.FileOutputStream;
import java.io.DataOutputStream;
import java.lang.reflect.*;

import sun.security.pkcs11.wrapper.CK_ATTRIBUTE;
import sun.security.pkcs11.wrapper.CK_C_INITIALIZE_ARGS;
import sun.security.pkcs11.wrapper.CK_MECHANISM;
import sun.security.pkcs11.wrapper.PKCS11;
import sun.security.pkcs11.wrapper.PKCS11Constants;
import sun.security.pkcs11.wrapper.PKCS11Exception;

public class sign_p11 
{
	public static void main(String[] args) throws IOException, PKCS11Exception
	{
		PKCS11 pkcs11;

		String osName = System.getProperty("os.name");
		String javaVersion = System.getProperty("java.version");
		System.out.println("Java version: " + javaVersion);

		try 
		{
			String libName = "libbeidpkcs11.so";
			if (-1 != osName.indexOf("Windows"))
				libName = "beidpkcs11.dll";
			else if (-1 != osName.indexOf("Mac"))
				libName = "libbeidpkcs11.dylib";

			Class pkcs11Class = Class.forName("sun.security.pkcs11.wrapper.PKCS11");
			if (javaVersion.startsWith("1.5."))
			{
				//////// For the sunpkcs11 in JDK 1.5 ////////
				Method getInstanceMethode = pkcs11Class.getDeclaredMethod("getInstance", new Class[] { String.class, CK_C_INITIALIZE_ARGS.class, boolean.class });
				pkcs11 = (PKCS11)getInstanceMethode.invoke(null, new Object[] { libName, null, false });
				//pkcs11 = PKCS11.getInstance(libName, null, false);
			}
			else
			{
				//////// For the sunpkcs11 in JDK 1.6 ////////
				Method getInstanceMethode = pkcs11Class.getDeclaredMethod("getInstance", new Class[] { String.class, String.class, CK_C_INITIALIZE_ARGS.class, boolean.class });
				pkcs11 = (PKCS11)getInstanceMethode.invoke(null, new Object[] { libName, "C_GetFunctionList", null, false });
				//pkcs11 = PKCS11.getInstance(libName, "C_GetFunctionList", null, false);
			}

			//Open the P11 session
			long p11_session = pkcs11.C_OpenSession(0, PKCS11Constants.CKF_SERIAL_SESSION, null, null);

			try 
			{
				//Find the signature private key
				CK_ATTRIBUTE[] attributes = new CK_ATTRIBUTE[2];
				attributes[0] = new CK_ATTRIBUTE();
				attributes[0].type = PKCS11Constants.CKA_CLASS;
				attributes[0].pValue = new Long(PKCS11Constants.CKO_PRIVATE_KEY);
				attributes[1] = new CK_ATTRIBUTE();
				attributes[1].type = PKCS11Constants.CKA_ID;
				attributes[1].pValue = 3;

				pkcs11.C_FindObjectsInit(p11_session, attributes);

				long[] keyHandles = pkcs11.C_FindObjects(p11_session, 1);
				long signatureKey = keyHandles[0];

				pkcs11.C_FindObjectsFinal(p11_session);

				//Open the data to sign
				File file = null;
				file = new File ("sign_p11.class");
				FileInputStream file_input = new FileInputStream (file);
				DataInputStream data_in    = new DataInputStream (file_input);

				byte[] data = new byte[(int)file.length()];
				data_in.read(data);
				data_in.close();

				//Initialize the signature
				CK_MECHANISM mechanism = new CK_MECHANISM();
				mechanism.mechanism = PKCS11Constants.CKM_SHA1_RSA_PKCS;
				mechanism.pParameter = null;
				pkcs11.C_SignInit(p11_session, mechanism, signatureKey);

				//Sign the data
				byte[] signature = pkcs11.C_Sign(p11_session, data);

				//Write the signature into a file
				file = new File ("sign_p11_java.sig");
				FileOutputStream file_output = new FileOutputStream (file);
				DataOutputStream data_out = new DataOutputStream (file_output);
				data_out.write(signature);
				file_output.close();

				System.out.println("Signing successful");
			}
			catch (Exception e)
			{
				System.out.println("[Catch] Exception: " + e.getMessage());
			}
			finally 
			{
				//Close the session
				pkcs11.C_CloseSession(p11_session);
			}
		}
		catch (Exception e)
		{
			System.out.println("[Catch] Exception: " + e.getMessage());
		}
	}
}