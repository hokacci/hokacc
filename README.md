# hokacc

以下のWebサイトを見ながら勉強用。

https://www.sigbus.info/compilerbook


ローカルで作業していたら、リポジトリが破損し、セクション9以前のコミットログがなくなりました。ぴえん。


## 必要なツール

- make: 色々なタスクをmakeで記述しているので必要
- docker: docker内でビルドを行うのに必要・ローカルで開発するなら不要


## セットアップする

ビルドを行うためのDockerイメージをビルドする。  
ちょっと時間がかかる。

```
make setup
```

## ビルドする

エフェメラルなDockerコンテナ内でビルドが行われる。

```
make
```

成果物は`build/hokacc`にできる。


## VSCodeの補完機能を使いながら開発する

以下で開発用のコンテナが起動するので、リモートエクスプローラー -> Containers -> hokacc-devenvにアタッチして、`/workspace`を開けばOK。  
コンテナ内でC++の拡張機能を入れれば最低限の補完は効くようになる。

```
make devenv-init
```

補完を利かせたいだけで環境を汚すのを厭わないなら、Dockerfile見ながら、ローカルにライブラリをインストールしてしまうのも手。


開発用コンテナを落とすときは、

```
make devenv-down
```

立ち上げ直すときは、

```
make devenv-up
```

などとする。

コンテナ内でのビルドは、

```
make build-local
```

とする。
