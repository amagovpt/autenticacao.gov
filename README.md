# Middleware Oficial de Identificação Eletrónica em Portugal - Cartão de Cidadão, da Chave Móvel Digital e Sistema de Certificação de atributos profissionais
## Introdução
### Objectivos

O objectivo deste projecto é disponibilizar uma nova versão de middleware que facilite e potencie a utilização do cartão de cidadão.

## SDK

Está disponível um SDK, com o objetivo de disponibilizar a integração com outras aplicações, das funcionalidades desenvolvidas no âmbito deste projeto.
Existem SDK's para as linguagens C, C++, JAVA, .NET e C#.

## Compilar o Middleware do Cartão de Cidadão

### Linux

1. Instalação das dependências.
   As dependências para a compilação do Middleware em Linux (nomes de pacotes para a distribuição Ubuntu18), são as seguintes:

   ```bash
   sudo apt install libpcsclite-dev libpoppler-qt5-dev libzip-dev libopenjp2-7-dev libpng-dev openjdk-8-jdk qtbase5-dev qt5-qmake qtbase5-private-dev qt5-default qtdeclarative5-dev qtquickcontrols2-5-dev qml-module-qtquick-controls2 libssl1.0-dev libxerces-c-dev libxml-security-c-dev swig libcurl4-nss-dev
   ```

2. Clonar o repositório

   ```bash
   git clone REPOSITORY_URL
   ```

3. Compilar o projecto na directoria eidmw

   ```bash
   cd eidmw
   qmake pteid-mw.pro
   make
   ```

4. Caso pretenda instalar a aplicação

   ```bash
   make install
   ```

   A aplicação utilitária será instalada em: /usr/local/bin/pteidgui

### Windows

Instruções para build em Windows (Visual Studio 2013)

É importante assegurar que as dependências são compiladas com a mesma versão de Visual Studio que será usada para compilar o Middleware.
Será assim necessário compilar as seguintes bibliotecas:

- openSSL 1.0.2 -  https://www.openssl.org/source/openssl-1.0.2p.tar.gz
- xerces-c -  https://xerces.apache.org/xerces-c/build-3.html#Windows
- libcurl -  https://curl.haxx.se/download.html
- zlib - https://sourceforge.net/projects/libpng/files/zlib/1.2.11/zlib-1.2.11.tar.gz/download?use_mirror=datapacket&download=
- libzip - https://libzip.org/download/
- libpng - http://www.libpng.org/pub/png/libpng.html
- openjpeg - https://www.openjpeg.org/

Para o caso do Qt estão disponíveis binários pré-compilados, poderá ser utilizado o instalador disponível em:  `https://download.qt.io/archive/online_installers/2.0/`

Ferramentas adicionais a instalar:

- 7-zip
- Windows Platform SDK
- Visual Studio
- doxygen - disponível em http://www.doxygen.nl/download.html
- Swigwin 2.0.12 - disponível em  https://sourceforge.net/projects/swig/files/swigwin/swigwin-2.0.12/
- Java JDK
- wix - disponível em https://wixtoolset.org/releases/
- Microsoft Merge Modules


Após configurado o ambiente conforme acima descrito, efetuar os seguintes passos:

1. Clonar repositório
2. Configurar os caminhos para as dependências (x86 e x64) e ferramentas no ficheiro `pteid-mw-pt\windows\set_path_autenticacao.gov.bat`
3. Abrir a Solução de VS2013 através do script `pteid-mw-pt\windows\ezbuild_sln_autenticacao.gov.bat
4. Seleccionar a configuração `Release | Win32` ou `Release | x64`
5. Compilar o projecto através do menu `Build solution`

Em alternativa, pode ser editado o script `pteid-mw-pt\windows\ezbuild_sln_autenticacao.gov.bat` para compilar na consola. As instruções estão no próprio ficheiro.

