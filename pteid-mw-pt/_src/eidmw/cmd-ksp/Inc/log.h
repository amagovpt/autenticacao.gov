/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2017 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2017 Luiz Lemos - <luiz.lemos@caixamagica.pt>
 * Copyright (C) 2020 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */

#include "..\..\common\Log.h"

//logging
#ifdef WIN32
#define _LOG_( buf, level, mod, format, ... ) { sprintf( buf, "%s() - ", __FUNCTION__ );                \
                                                    sprintf( &buf[strlen(buf)], format, __VA_ARGS__ );      \
                                                    MWLOG( level, mod, buf);                                \
                                                    printf( "%s\n", buf ); }
#define MWLOG_ERR( buf, format, ...   )     _LOG_( buf, LEV_ERROR, MOD_KSP, format, __VA_ARGS__ )
#define MWLOG_WARN( buf, format, ...  )     _LOG_( buf, LEV_WARN , MOD_KSP, format, __VA_ARGS__ )
#define MWLOG_INFO( buf, format, ...  )     _LOG_( buf, LEV_INFO , MOD_KSP, format, __VA_ARGS__ )
#define MWLOG_DEBUG( buf, format, ... )     _LOG_( buf, LEV_DEBUG, MOD_KSP, format, __VA_ARGS__ )
#endif