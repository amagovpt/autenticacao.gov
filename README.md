# Middleware Oficial de Identificação Eletrónica em Portugal - Cartão de Cidadão, da Chave Móvel Digital e Sistema de Certificação de atributos profissionais
## Introdução
### Objectivos

O objectivo deste projecto é disponibilizar uma nova versão de middleware que facilite e potencie a utilização do Cartão de Cidadão
e da assinatura com Chave Móvel Digital

## Manual de Utilização

O Manual de Utilização pretende descrever todas as funcionalidades providenciadas pelo middleware do Cartão de Cidadão.

[Manual de Utilização (online)](https://amagovpt.github.io/autenticacao.gov/user_manual.html)

[Manual de Utilização (download pdf)](docs/Manual_de_Utilizacao_v3.pdf)

## SDK

Está disponível um SDK, com o objetivo de disponibilizar a integração com outras aplicações, das funcionalidades desenvolvidas no âmbito deste projeto.
Existem SDK's para as linguagens C, C++, JAVA e C#.

Poderá aceder ao manual no seguinte URL:

[Manual do SDK (online)](https://amagovpt.github.io/autenticacao.gov/manual_sdk.html)

[Manual do SDK (download pdf)](docs/Manual_de_SDK.pdf)

A documentação da API disponibilizada pelo SDK está disponível nos seguintes URLs:


[Documentação Técnica do SDK C++ (online)](https://amagovpt.github.io/autenticacao.gov/sdk/cpp/)

[Documentação Técnica do SDK Java (online)](https://amagovpt.github.io/autenticacao.gov/sdk/java/)

## Compilar o Middleware do Cartão de Cidadão

### Linux

1. Instalação das dependências.
   As dependências para a compilação do Middleware em Linux (nomes de pacotes válidos para a distribuição Ubuntu 19.10), são as seguintes:

   ```bash
   sudo apt install libpcsclite-dev libpoppler-qt5-dev libzip-dev libopenjp2-7-dev libpng-dev openjdk-11-jdk qtbase5-dev qt5-qmake qtbase5-private-dev qt5-default qtdeclarative5-dev qtquickcontrols2-5-dev qml-module-qtquick-controls2 libssl-dev libxerces-c-dev libxml-security-c-dev swig libcurl4-nss-dev libnsspem
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

   A aplicação utilitária será instalada em: /usr/local/bin/eidguiV2

   As dependências para a execução do Middleware na distribuição Ubuntu 19.10 estão disponíveis em "Depends" no seguinte URL: [Dependências](pteid-mw-pt/_src/eidmw/debian/control)


### Windows

Instruções para compilação em Windows (Visual Studio 2017)

É importante assegurar que as dependências são compiladas com a mesma versão de Visual Studio que será usada para compilar o Middleware.
Será assim necessário compilar as seguintes bibliotecas (versões utilizadas para a última release Windows):

- openSSL 1.1.1 - https://www.openssl.org/source/openssl-1.1.1f.tar.gz
- xerces-c 3.2.3 - https://xerces.apache.org/xerces-c/build-3.html
- libcurl 7.69.1 - https://curl.haxx.se/download.html
- zlib 1.2.11 - http://www.zlib.net/zlib-1.2.11.tar.gz
- libzip 1.5.2 - https://libzip.org/download/
- libpng 1.6.37 - http://www.libpng.org/pub/png/libpng.html
- openjpeg 2.3 - https://www.openjpeg.org/
- libxml-security-c 2.0.2 - http://archive.apache.org/dist/santuario/c-library/xml-security-c-2.0.2.zip

Para compilar a aplicação GUI, projeto disponível em eidmw/eidguiV2, é necessária uma dependência adicional:

- poppler-qt5 0.87 - https://poppler.freedesktop.org/

Para o caso do Qt5 (versão igual ou superior a 5.12) estão disponíveis binários pré-compilados, poderá ser utilizado o instalador disponível em:  `https://download.qt.io/archive/online_installers/2.0/`

Ferramentas adicionais a instalar:

- 7-zip
- Windows Platform SDK
- Windows Cryptographic Provider Development Kit
- Visual Studio
- doxygen - disponível em http://www.doxygen.nl/download.html
- Swigwin 4.0.1 - disponível em  https://sourceforge.net/projects/swig/files/swigwin/swigwin-4.0.1/
- Java JDK
- Wix - disponível em https://wixtoolset.org/releases/
- Wix Visual Studio Extension - https://marketplace.visualstudio.com/items?itemName=WixToolset.WixToolsetVisualStudio2017Extension


Após configurado o ambiente conforme acima descrito, efetuar os seguintes passos:

1. Clonar repositório
2. Configurar os caminhos para as dependências (x86 e x64) e ferramentas no ficheiro `pteid-mw-pt\windows\set_path_autenticacao.gov.bat`
3. Abrir a Solução de VS2017 através do script `pteid-mw-pt\windows\ezbuild_sln_autenticacao.gov.bat
4. Seleccionar a configuração `Release | Win32` ou `Release | x64`
5. Compilar o projecto através do menu `Build solution`

Em alternativa, pode ser editado o script `pteid-mw-pt\windows\ezbuild_sln_autenticacao.gov.bat` para compilar na consola. As instruções estão no próprio ficheiro.

## Contactos
Para questões, sugestões ou comentários envie um e-mail para info.cidadao@ama.pt.
