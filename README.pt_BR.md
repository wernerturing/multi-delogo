# multi-delogo

multi-delogo é um programa que lhe ajuda a remover logos ou marcas-d'água de vídeos. Mesmo se a posição do logo muda de tempos em tempos, multi-delogo lhe permite marcar todas as posições e gerar um vídeo sem os logos.

Existe uma funcionalidade para detecção automática de logos e suas posições, especialmente logos formados por textos.

Para mais detalhes e instruções de uso, veja [aqui](docs/pt_BR/README.md).


## Instalação no Windows

Baixe a última versão da [página de releaes](https://github.com/wernerturing/multi-delogo/releases) e descompacte o arquivo. Execute o arquivo `multi-delogo.exe` para iniciar a aplicação.


## Instalação no Linux

Para compilar o software no Linux, você precisará dos arquivos de desenvolvimento para as seguintes bibliotecas:

* gtkmm
* goocanvasmm
* opencv
* boost

Você também precisará de um compilar C++11 e do `make`.

Baixe a última versão da [página de releaes](https://github.com/wernerturing/multi-delogo/releases), descompacte o arquivo, e rode

```sh
./configure
make
make install
```

Em seguida, execute `multi-delogo` para iniciar a aplicação.


## Instalação no Mac OS

Há uma [fórmula do Homebrew](https://github.com/wernerturing/homebrew-multi-delogo) para instalar o multi-delogo automaticamente.


## Instalação do github

O processo é muito parecido com a instação no Linux descrita acima. Você precisará de algumas dependências adicionais:

* autoconf
* automake
* autopoint
* gettext

Faça o clone do projeto com

```sh
git clone https://github.com/wernerturing/multi-delogo.git
```

Entre no diretório `multi-delogo`, execute `autogen.sh` para gerar o script `configure` e os Makefiles, e finalment execute

```sh
./configure
make
make install
```

Em seguida, execute `multi-delogo` para iniciar a aplicação.


## Bugs, sugestões, etc.

Você pode usar a página de [issues](https://github.com/wernerturing/multi-delogo/issues) para relatar bugs e sugestões.

Pull requests são bem vindos; por favor tente seguir o estilo de código.

Você também pode mandar um email para werner.turing@protonmail.com.


## Copyright

multi-delogo é Copyright (C) 2018-2019 Werner Turing <werner.turing@protonmail.com>

multi-delogo é um software livre: você pode redistribui-lo e/ou modificá-lo de acordo com os termos da GNU General Public License publicada pela Free Software Foundation, ou a versão 3 da Licença, ou (caso queira) qualquer versão posterior.

multi-delogo é distribuído esperando que seja útil, mas SEM NENHUMA GARANTIA. Veja a [GNU General Public License (em inglês)](COPYING) para mais detalhes.
