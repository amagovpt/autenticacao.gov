/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package pteidlib;

/**
 *
 * @author ruim
 */
public class PteidException extends Exception
{
  private int error_code;
  private String message;

  public PteidException()
  {
  	this.message = "Generic error";
  	this.error_code = 0;
  }
  
  public PteidException(int error_code)
  {
    this.error_code = error_code;
    String str = "Error code : " + Integer.toString(error_code);
    this.message = str;
  }
  
  public int getStatus()
  {
    return this.error_code;
  }
  
  public String getMessage()
  {
    return this.message;
  }
}
