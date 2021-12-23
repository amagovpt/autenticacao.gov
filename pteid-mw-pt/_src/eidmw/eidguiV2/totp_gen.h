/*-****************************************************************************

 * Copyright (C) 2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2018 Adriano Campos - <adrianoribeirocampos@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/
#ifndef TOTP_GEN_H
#define TOTP_GEN_H

//Implemented in totp_gen.cpp
std::string generateTOTP(std::string secretKey, unsigned int digits, unsigned int step_time, time_t my_time);
#endif // TOTP_GEN_H
