package eu.europa.ec.markt.dss.applet.util;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;

import org.bouncycastle.cms.CMSException;
import org.bouncycastle.cms.CMSSignedData;

import eu.europa.ec.markt.dss.DSSUtils;
import eu.europa.ec.markt.dss.applet.main.FileType;

/**
 * 
 * TODO
 * 
 * <p>
 * DISCLAIMER: Project owner DG-MARKT.
 * 
 * @version $Revision: 1016 $ - $Date: 2011-06-17 15:30:45 +0200 (Fri, 17 Jun 2011) $
 * @author <a href="mailto:dgmarkt.Project-DSS@arhs-developments.com">ARHS Developments</a>
 */
public final class FileTypeDetectorUtils {
   /**
    * 
    * @param file
    * @return
    * @throws IOException
    */
   private static String extractPreambleString(final File file) throws IOException {

      FileInputStream inputStream = null;

      try {
         inputStream = new FileInputStream(file);

         final byte[] preamble = new byte[5];
         final int read = inputStream.read(preamble);
         if (read < 5) {
            throw new RuntimeException();
         }

         return new String(preamble);
      } finally {
         DSSUtils.closeQuietly(inputStream);
      }
   }

   private static boolean isASiC(final File file) {
      return file.getName().toLowerCase().endsWith(".asics");
   }

   /**
    * 
    * @param file
    * @return
    * @throws FileNotFoundException
    */
   private static boolean isCMS(final File file) throws FileNotFoundException {
      FileInputStream inputStream = null;

      try {
         inputStream = new FileInputStream(file);
         new CMSSignedData(inputStream);
         return true;
      } catch (final CMSException e) {
         return false;
      } finally {
         DSSUtils.closeQuietly(inputStream);
      }
   }

   /**
    * 
    * @param preamble
    * @return
    */
   private static boolean isPDF(final String preamble) {
      return preamble.equals("%PDF-");
   }

   /**
    * 
    * @param preamble
    * @return
    */
   private static boolean isXML(final String preamble) {
      return preamble.equals("<?xml");
   }

   /**
    * 
    * @param file
    * @return
    */
   public static FileType resolveFiletype(final File file) {

      try {
         final String preamble = extractPreambleString(file);

         // XML
         if (isXML(preamble)) {
            return FileType.XML;
         }
         // PDF
         if (isPDF(preamble)) {
            return FileType.PDF;
         }

         if (isASiC(file)) {
            return FileType.ASiCS;
         }

         try {
            if (isCMS(file)) {
               return FileType.CMS;
            }
            return FileType.BINARY;
         } catch (final Exception e) {
            return FileType.BINARY;
         }

      } catch (final IOException e) {
         throw new RuntimeException("Cannot determine the mime/type");
      }
   }

   /**
    * 
    * The default constructor for FileTypeDetectorUtils.
    */
   private FileTypeDetectorUtils() {

   }
}
