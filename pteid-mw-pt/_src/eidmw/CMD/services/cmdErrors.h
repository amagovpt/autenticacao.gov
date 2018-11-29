#ifndef CMD_ERRORS_H
#define CMD_ERRORS_H

#define ERR_ADDR_USER_BASE      1000
#define ERR_NONE                0x00
#define ERR_NULL_HANDLER        (ERR_ADDR_USER_BASE + 0x01)
#define ERR_NULL_CARD           (ERR_ADDR_USER_BASE + 0x02)
#define ERR_NULL_PDF            (ERR_ADDR_USER_BASE + 0x03)
#define ERR_NULL_PDF_HANDLER    (ERR_ADDR_USER_BASE + 0x04)
#define ERR_INV_CERTIFICATE     (ERR_ADDR_USER_BASE + 0x05)
#define ERR_INV_CERTIFICATE_CA  (ERR_ADDR_USER_BASE + 0x06)
#define ERR_GET_CERTIFICATE     (ERR_ADDR_USER_BASE + 0x07)
#define ERR_SEND_HASH           (ERR_ADDR_USER_BASE + 0x08)
#define ERR_GET_SIGNATURE       (ERR_ADDR_USER_BASE + 0x09)
#define ERR_SIGN_PDF            (ERR_ADDR_USER_BASE + 0x0A)
#define ERR_GET_HASH            (ERR_ADDR_USER_BASE + 0x0B)
#define ERR_SIGN_CLOSE          (ERR_ADDR_USER_BASE + 0x0C)
#define ERR_INV_USERID          (ERR_ADDR_USER_BASE + 0x0D)
#define ERR_INV_USERPIN         (ERR_ADDR_USER_BASE + 0x0E)
#define ERR_INV_HASH            (ERR_ADDR_USER_BASE + 0x0F)
#define ERR_INV_CODE            (ERR_ADDR_USER_BASE + 0x10)
#define ERR_SIZE                (ERR_ADDR_USER_BASE + 0x11)
#define ERR_NULL_DATA           (ERR_ADDR_USER_BASE + 0x12)
#define ERR_INV_DATA            (ERR_ADDR_USER_BASE + 0x13)

#define SOAP_SUCCESS_CODE                   200
#define IS_SOAP_ERROR(error)    ( ( error != ERR_NONE ) && ( error != SOAP_SUCCESS_CODE ) && ( error < ERR_ADDR_USER_BASE ) )

#define SOAP_ERR_SERVICE_FAIL               500 // NOT TESTED
#define SOAP_ERR_INVALID_PIN                801 
#define SOAP_ERR_INVALID_OTP                802 // NOT TESTED
#define SOAP_ERR_OTP_VALIDATION_ERROR       816 // NOT TESTED
#define SOAP_ERR_INACTIVE_SERVICE           817 // NOT TESTED
#define SOAP_ERR_GENERIC                    900 // NOT TESTED

#endif /* CMD_ERRORS_H */
