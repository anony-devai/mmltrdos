/* ============================================================
 * mmltrd16.c  (MML Transposer リアルモード MS-DOS 16bit CUI版)
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mmleng16.h"		//16bitヘッダ

char huge text[MAX_TEXT];	//hugeをつける
char huge outbuf[MAX_OUT];	//hugeをつける

/* 整数判定 */
int is_integer_arg(const char* s)
{
    int i = 0;

    if (s[0] == '+' || s[0] == '-') {
        i = 1;
    }

    if (s[i] == '\0') return 0;

    for (; s[i] != '\0'; i++) {
        if (s[i] < '0' || s[i] > '9') return 0;
    }

    return 1;
}

void print_usage(void) {
    fprintf(stdout,
        "使い方:\n"
        "  mmltrd16 [options] <input.mml> [shift] [output.mml]\n"
        "\n"
        "オプション:\n"
        "  -i <file>     入力ファイル\n"
        "  -o <file>     出力ファイル\n"
        "  -s <shift>    移調量（-12～+12）\n"
        "  -p            Pure モード（整形なし）\n"
        "  -f            FMT  モード（整形あり）\n"
        "  -r            相対音域（先頭 oX、以降 < >）\n"
        "  -a            絶対音域（全て oX）\n"
        "  -d            D チャンネル移調（ノイズchも移調）\n"
        "  -h            詳細ヘルプ\n"
        "\n"
        "このプログラムは NSF用 MML 移調ツールです。\n"
        "ヘルプは -h | more でのページ出力を推奨します。\n"
    );
}

void print_help_detail(void)
{
    fprintf(stdout,
        "使い方:"
        "  mmltrd16 [options] <input.mml> [shift] [output.mml]\n"
        "\n"
        "オプション:        （すべて省略可）\n"
        "  -i <file>        入力ファイルを指定\n"
        "  -o <file>        出力ファイルを指定\n"
        "  -s <shift>       移調量（-12～+12） 0 は移調なし（+ は省略可）\n"
        "  -p, --pure       Pure モード（整形なし）\n"
        "  -f, --fmt        FMT  モード（整形あり）\n"
        "  -r, --relative   相対音域指定（先頭のみ oX、以降は < >）\n"
        "  -a, --absolute   絶対音域指定（すべて oX）\n"
        "  -d, --dch        D チャンネル移調（ノイズchも移調量に応じて移調）\n"
        "  -h, --help       このヘルプを出力\n"
        "例:\n"
        "  mmltrd16 input.mml\n"
        "         → そのまま  標準出力  へ 出力  （移調処理なし画面出力）\n"
        "\n"
        "  mmltrd16 input.mml output.mml\n"
        "         → そのまま output.mml へ コピー（移調処理なし単純コピー）\n"
        "\n"
        "  mmltrd16 input.mml -s 0\n"
        "         → 移調量 0 で 処理結果を 標準出力へ 出力（モード省略時は Pure）\n"
        "\n"
        "  mmltrd16 input.mml 5\n"
        "         → +5 移調して 処理結果を 標準出力へ 出力（-s は省略可）\n"
        "\n"
        "モード説明:\n"
        "※ Pure / FMT モードでは、音符とオクターブを\n"
        "   元の MML の意図を保ちながら自動的に振り直します。\n"
        "\n"
        "※ -p / -f は 整形方式（ Pure＝整形なし / FMT＝整形あり ）を指定します。\n"
        "   -r / -a は 音域方式（ 相対音域 [<>]  / 絶対音域 [oX] ）を指定します。\n"
        "   これらは組み合わせて使用できます。\n"
        "\n"
        "例:\n"
        "  mmltrd16 input.mml -2 output.mml -p\n"
        "         → -2 移調して Pure（整形なし）で 音域は 自動振り直しで 出力\n"
        "\n"
        "  mmltrd16 input.mml +3 output.mml -p -a\n"
        "         → +3 移調して Pure（整形なし）で 音域は 絶対音域[oX]で 出力\n"
        "\n"
        "  mmltrd16 input.mml +7 output.mml -f\n"
        "         → +7 移調して FMT （整形あり）で 音域は 自動振り直しで 出力\n"
        "\n"
        "  mmltrd16 input.mml -5 output.mml -f -r -d\n"
        "         → -5 移調して FMT、相対音域[<>]、Dチャンネルも移調して 出力\n"
        "\n"
        "※ -d で D チャンネル移調しても オクターブは o0 固定となります。\n"
    );
}

int main(int argc, char* argv[])
{
    const char* infile = NULL;
    const char* outfile = NULL;

    FILE* fp;
    size_t len;
    size_t written; /* DEBUG用 */

    int shift = 0;
    int shift_specified = 0;

    int fmt_flag  = 0;
    int pure_flag = 0;
    int rel_flag  = 0;
    int abs_flag  = 0;
    int dch_flag  = 0; /* [NEW] -d / --dch フラグ用（旧 noise_flag） */

    int i;
    int show_help = 0;
    FILE* outfp;

    const char* file_candidates[16];
    int file_count = 0;
    int outlen;

    /* エラー詳細構造体を宣言 (C89準拠のため関数先頭ブロックに配置) */
    MMLErrorInfo err_info;

    /* 引数なし → ヘルプ */
    if (argc < 2) {
        print_usage();
        return 1;
    }

    /* ヘルプ最優先 */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            show_help = 1;
            break;
        }
    }
    if (show_help) {
        print_help_detail();
        return 0;
    }

    /* ------------------------------------------------------------
       -i / -o を先に処理
       ------------------------------------------------------------ */
    for (i = 1; i < argc; i++) {

        if (strcmp(argv[i], "-i") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "エラー: -i の後にファイル名を指定してください。\n");
                return 1;
            }
            infile = argv[i + 1];
            i++;
            continue;
        }

        if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "エラー: -o の後にファイル名を指定してください。\n");
                return 1;
            }
            outfile = argv[i + 1];
            i++;
            continue;
        }
    }

    /* ------------------------------------------------------------
       残りの引数を処理
       ------------------------------------------------------------ */
    for (i = 1; i < argc; i++) {

        if (argv[i][0] == '-') {

            /* すでに処理済みの -i / -o はスキップ */
            if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "-o") == 0) {
                i++;
                continue;
            }

            /* Pure / FMT */
            if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--pure") == 0) {
                if (fmt_flag) {
                    fprintf(stderr, "エラー: --pure と --fmt は同時に指定できません。\n");
                    return 1;
                }
                pure_flag = 1;
                continue;
            }

            if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--fmt") == 0) {
                if (pure_flag) {
                    fprintf(stderr, "エラー: --pure と --fmt は同時に指定できません。\n");
                    return 1;
                }
                fmt_flag = 1;
                continue;
            }

            /* relative / absolute */
            if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--relative") == 0) {
                if (abs_flag) {
                    fprintf(stderr, "エラー: -r と -a は同時に指定できません。\n");
                    return 1;
                }
                rel_flag = 1;
                continue;
            }

            if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--absolute") == 0) {
                if (rel_flag) {
                    fprintf(stderr, "エラー: -r と -a は同時に指定できません。\n");
                    return 1;
                }
                abs_flag = 1;
                continue;
            }

            /* [NEW] -d / --dch (D chシフト) オプションの解析 */
            if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--dch") == 0) {
                dch_flag = 1;
                continue;
            }

            /* -s <shift> */
            if (strcmp(argv[i], "-s") == 0) {
                long s;
                char* endptr;

                if (i + 1 >= argc) {
                    fprintf(stderr, "エラー: -s の後に移調量を指定してください。\n");
                    return 1;
                }

                if (shift_specified) {
                    fprintf(stderr, "エラー: shift が複数指定されています。\n");
                    return 1;
                }

                s = strtol(argv[i + 1], &endptr, 10);
                if (*endptr != '\0') {
                    fprintf(stderr, "エラー: -s の移調量は整数で指定してください。\n");
                    return 1;
                }
                if (s < -12 || s > 12) {
                    fprintf(stderr, "エラー: shift は -12～+12 です。\n");
                    return 1;
                }

                shift = (int)s;
                shift_specified = 1;
                i++;
                continue;
            }

            /* 裸の整数を shift として解釈 */
            if (is_integer_arg(argv[i])) {
                long s;
                if (shift_specified) {
                    fprintf(stderr, "エラー: shift が複数指定されています。\n");
                    return 1;
                }
                s = strtol(argv[i], NULL, 10);
                if (s < -12 || s > 12) {
                    fprintf(stderr, "エラー: shift は -12～+12 です。\n");
                    return 1;
                }
                shift = (int)s;
                shift_specified = 1;
                continue;
            }

            fprintf(stderr, "エラー: 不明なオプションです: %s\n", argv[i]);
            return 1;
        }

        /* ここに来るのは非オプション引数 */

        /* 裸の整数 → shift */
        if (is_integer_arg(argv[i])) {
            long s;
            if (shift_specified) {
                fprintf(stderr, "エラー: shift が複数指定されています。\n");
                return 1;
            }
            s = strtol(argv[i], NULL, 10);
            if (s < -12 || s > 12) {
                fprintf(stderr, "エラー: shift は -12～+12 です。\n");
                return 1;
            }
            shift = (int)s;
            shift_specified = 1;
            continue;
        }

        /* ファイル候補として保存 */
        if (file_count < 16) {
            file_candidates[file_count++] = argv[i];
        }
    }

    /* -i / -o がない場合は暗黙ルールで解釈 */
    if (!infile) {
        if (file_count >= 1) infile = file_candidates[0];
    }

    if (!outfile) {
        if (file_count >= 2) outfile = file_candidates[1];
    }

    if (!infile) {
        fprintf(stderr, "エラー: 入力ファイルが指定されていません。\n");
        print_usage();
        return 1;
    }

    if (outfile && strcmp(infile, outfile) == 0) {
        fprintf(stderr, "エラー: 入力と出力が同じです。\n");
        return 1;
    }

    /* ------------------------------------------------------------
       入力読み込み（MAX_TEXT と連動したサイズチェック付き）
    ------------------------------------------------------------ */
    fp = fopen(infile, "rb");
    if (!fp) {
        fprintf(stderr, "エラー: ファイルを開けません: %s\n", infile);
        return 1;
    }

    /* ファイルサイズチェック */
    if (fseek(fp, 0, SEEK_END) != 0) {
        fprintf(stderr, "エラー: ファイルサイズを取得できません: %s\n", infile);
        fclose(fp);
        return 1;
    }

    {
        long fsize = ftell(fp);
        if (fsize < 0) {
            fprintf(stderr, "エラー: ファイルサイズを取得できません: %s\n", infile);
            fclose(fp);
            return 1;
        }

        if (fsize >= MAX_TEXT) {
            fprintf(stderr,
                "エラー: 入力ファイルが大きすぎます (%ld バイト)。\n"
                "        最大 %d バイトまでです。\n",
                fsize, MAX_TEXT - 1);
            fclose(fp);
            return 1;
        }

        if (fseek(fp, 0, SEEK_SET) != 0) {
            fprintf(stderr, "エラー: ファイル位置を先頭に戻せません: %s\n", infile);
            fclose(fp);
            return 1;
        }
    }

    /* 実際の読み込み */
    len = fread(text, 1, MAX_TEXT - 1, fp);
    fclose(fp);
    text[len] = '\0';

    /* ------------------------------------------------------------
       処理が必要かどうか判定 (C89準拠のため変数宣言はブロック先頭に)
       ------------------------------------------------------------ */
    {
        int need_process = 0;

        if (shift_specified || fmt_flag || pure_flag || rel_flag || abs_flag || dch_flag) {
            need_process = 1;
        }

        /* 処理不要 → 単純コピー */
        if (!need_process) {
            outfp = outfile ? fopen(outfile, "w") : stdout;
            if (!outfp) {
                fprintf(stderr, "エラー: 出力ファイルを開けません。\n");
                return 1;
            }
            fprintf(outfp, "%s", text);
            if (outfile) fclose(outfp);
            return 0;
        }
    }

    /* shift 未指定なら 0 とみなす（Pure / FMT のトリガとして扱う） */
    if (!shift_specified) {
        shift = 0;
    }

    /* ------------------------------------------------------------
       mml_process() 呼び出し (C89準拠のため変数宣言はブロック先頭に)
       ------------------------------------------------------------ */
    {
        int mode;
        int base_mode;

        /* Pure / FMT のベースモード決定 */
        if (fmt_flag) {
            base_mode = MODE_FMT;      /* 4 */
        } else {
            base_mode = MODE_PURE;     /* 0 */
        }

        mode = base_mode;

        /* Rel / Abs ビット付与 */
        if (rel_flag) {
            mode |= 2;                 /* REL ビット */
        } else if (abs_flag) {
            mode |= 1;                 /* ABS ビット */
        }

        /* D chシフトビット(8) 付与 */
        if (dch_flag) {
            mode |= 8;
        }

        /* 第6引数に &err_info を渡す */
        outlen = mml_process(text, shift, mode, outbuf, sizeof(outbuf), &err_info);
    }

    /* オクターブ限界突破（-10）等の詳細エラーハンドリングを適用 */
    if (outlen < 0) {
        if (outlen == MML_ERR_OCTAVE_OUT_OF_RANGE) {
            fprintf(stderr, "エラー: 移調により各音源のオクターブ限界を突破しました。\n");
            fprintf(stderr, "        発生場所: チャンネル '%c', %d 行目 (計算値: o%d)\n",
                    err_info.channel_char, err_info.line_number, err_info.calculated_value);
        } else {
            fprintf(stderr, "エラー: MML 処理に失敗しました (コード %d)\n", outlen);
        }
        return 1;
    }

    if (outlen >= (int)sizeof(outbuf)) {
        fprintf(stderr, "エラー: 出力バッファが不足しています。\n");
        return 1;
    }
    outbuf[outlen] = '\0';

    outfp = outfile ? fopen(outfile, "w") : stdout;
    if (!outfp) {
        fprintf(stderr, "エラー: 出力ファイルを開けません。\n");
        return 1;
    }

    written = fwrite(outbuf, 1, outlen, outfp);

    if (outfile) {
        fclose(outfp);
    }

    return 0;
}
