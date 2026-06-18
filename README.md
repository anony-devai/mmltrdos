# MML Transposer DOS (MMLTRD16)

NSF 用 MML ファイル移調ツール「MML Transposer」の  
16bit リアルモード MS-DOS 版（CUI版）です。

## 対応環境

- 16bit リアルモード MS-DOS  
- Windows 95 の MS-DOS モード（動作確認済み）

## エンジン構成

- 16bit 向けにメモリ使用量を最適化したエンジン `mmleng16.c` を使用しています。

## 使い方

```bash
mmltrd16 [options] <input.mml> [shift] [output.mml]
```

### オプション

- `-i <file>` 入力ファイル  
- `-o <file>` 出力ファイル  
- `-s <shift>` 移調量（-12～+12、0 は移調なし、`+` は省略可）  
- `-p`, `--pure` Pure モード（整形なし）  
- `-f`, `--fmt` FMT モード（整形あり）  
- `-r`, `--relative` 相対音域指定（先頭のみ `oX`、以降は `<` `>`）  
- `-a`, `--absolute` 絶対音域指定（すべて `oX`）  
- `-d`, `--dch` D チャンネル移調（ノイズ ch も移調量に応じて移調）  
- `-h`, `--help` ヘルプを表示

### 使用例

```bash
mmltrd16 input.mml
mmltrd16 input.mml output.mml
mmltrd16 input.mml -s 0
mmltrd16 input.mml 5
mmltrd16 input.mml -2 output.mml -p
mmltrd16 input.mml +3 output.mml -p -a
mmltrd16 input.mml +7 output.mml -f
mmltrd16 input.mml -5 output.mml -f -r -d
```

### モード説明

- **Pure / FMT モード**  
  元の MML の意図を保ちながら、音符とオクターブを自動的に振り直します。  
  - `-p` / `--pure` 整形なし  
  - `-f` / `--fmt` 整形あり  

- **音域指定**  
  - `-r` / `--relative` 相対音域（`<>`）  
  - `-a` / `--absolute` 絶対音域（`oX`）  
  これらは `-p` / `-f` と組み合わせて使用できます。

- **D チャンネル移調**  
  - `-d` / `--dch` を指定すると、ノイズチャンネルも移調します。  
  - オクターブは `o0` 固定です。

## 備考

このプログラムは NSF 用 MML 移調ツールです。  
ヘルプは `mmltrd16 -h | more` でページ送りしながら読むことを推奨します。

このコードは全て Copilot を活用して作成しています。  
予期せぬ不具合はご容赦ください。
