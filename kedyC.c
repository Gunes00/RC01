#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAKSIMUM_TOKEN_UZUNLUGU 64
#define MAKSIMUM_DEGISKENLER 100

typedef enum {
    TOKEN_SAYI,
    TOKEN_KIMLIK,
    TOKEN_ATA,
    TOKEN_ARTI,
    TOKEN_EKSI,
    TOKEN_EOF,
    TOKEN_NOKTALI_VIRGUL,
    TOKEN_MIRR,
    TOKEN_PARANTEZ_SOLDAN,
    TOKEN_PARANTEZ_SAGDAN,
} TokenTuru;

typedef struct {
    TokenTuru tur;
    char deger[MAKSIMUM_TOKEN_UZUNLUGU];
} Token;

typedef enum {
    AST_DEGISKEN_ATA,
    AST_SAYI,
    AST_OPERATOR,
    AST_MIRR_CAGRISI,
} ASTDugumTuru;

typedef struct ASTDugum {
    ASTDugumTuru tur;
    char deger[MAKSIMUM_TOKEN_UZUNLUGU];
    struct ASTDugum* sol;
    struct ASTDugum* sag;
} ASTDugum;

typedef struct {
    char isim[MAKSIMUM_TOKEN_UZUNLUGU];
    int deger;
} Degisken;

Degisken degiskenler[MAKSIMUM_DEGISKENLER];
int degisken_sayisi = 0;

Token sonrakiTokenuAl(FILE *dosya) {
    Token token = {TOKEN_EOF, ""};
    char karakter;

    while ((karakter = fgetc(dosya)) != EOF) {
        if (karakter == ' ' || karakter == '\t' || karakter == '\n') {
            continue;
        }

        if (isdigit(karakter)) {
            int i = 0;
            do {
                token.deger[i++] = karakter;
                karakter = fgetc(dosya);
            } while (isdigit(karakter));
            token.deger[i] = '\0';
            ungetc(karakter, dosya);
            token.tur = TOKEN_SAYI;
            return token;
        }

        if (isalpha(karakter)) {
            int i = 0;
            do {
                token.deger[i++] = karakter;
                karakter = fgetc(dosya);
            } while (isalnum(karakter));
            token.deger[i] = '\0';
            ungetc(karakter, dosya);
            token.tur = TOKEN_KIMLIK;
            return token;
        }

        if (karakter == '=') {
            token.deger[0] = '=';
            token.deger[1] = '\0';
            token.tur = TOKEN_ATA;
            return token;
        }

        if (karakter == '+') {
            token.deger[0] = '+';
            token.deger[1] = '\0';
            token.tur = TOKEN_ARTI;
            return token;
        }
        if (karakter == '-') {
            token.deger[0] = '-';
            token.deger[1] = '\0';
            token.tur = TOKEN_EKSI;
            return token;
        }

        if (karakter == ';') {
            token.deger[0] = ';';
            token.deger[1] = '\0';
            token.tur = TOKEN_NOKTALI_VIRGUL;
            return token;
        }

        if (karakter == 'm' || karakter == 'M') {
            karakter = fgetc(dosya);
            if (karakter == 'i' || karakter == 'I') {
                karakter = fgetc(dosya);
                if (karakter == 'r' || karakter == 'R') {
                    karakter = fgetc(dosya);
                    if (karakter == 'r' || karakter == 'R') {
                        karakter = fgetc(dosya);
                        if (karakter == '(') {
                            token.tur = TOKEN_MIRR;
                            return token;
                        }
                    }
                }
            }
        }

        if (karakter == '(') {
            token.deger[0] = '(';
            token.deger[1] = '\0';
            token.tur = TOKEN_PARANTEZ_SOLDAN;
            return token;
        }
        if (karakter == ')') {
            token.deger[0] = ')';
            token.deger[1] = '\0';
            token.tur = TOKEN_PARANTEZ_SAGDAN;
            return token;
        }
    }

    return token;
}

int degiskenDegeriAl(const char* isim) {
    for (int i = 0; i < degisken_sayisi; i++) {
        if (strcmp(degiskenler[i].isim, isim) == 0) {
            return degiskenler[i].deger;
        }
    }
    return 0;
}

void degiskenDegeriAta(const char* isim, int deger) {
    for (int i = 0; i < degisken_sayisi; i++) {
        if (strcmp(degiskenler[i].isim, isim) == 0) {
            degiskenler[i].deger = deger;
            return;
        }
    }
    strcpy(degiskenler[degisken_sayisi].isim, isim);
    degiskenler[degisken_sayisi].deger = deger;
    degisken_sayisi++;
}

void mirr(char* degisken_ismi) {
    int deger = degiskenDegeriAl(degisken_ismi);
    if (deger != 0) {
        printf("%d\n", deger);
    } else {
        printf("Hata: Degisken '%s' bulunamadi.\n", degisken_ismi);
    }
}

int ifadeninDegeriniHesapla(ASTDugum* dugum) {
    if (dugum == NULL) return 0;

    if (dugum->tur == AST_SAYI) {
        return atoi(dugum->deger);
    }
    if (dugum->tur == AST_OPERATOR) {
        int solDeger = ifadeninDegeriniHesapla(dugum->sol);
        int sagDeger = ifadeninDegeriniHesapla(dugum->sag);
        if (strcmp(dugum->deger, "+") == 0) {
            return solDeger + sagDeger;
        }
        if (strcmp(dugum->deger, "-") == 0) {
            return solDeger - sagDeger;
        }
    }
    if (dugum->tur == AST_DEGISKEN_ATA) {
        return degiskenDegeriAl(dugum->deger);
    }
    return 0;
}

void astYorumla(ASTDugum* dugum) {
    if (dugum == NULL) return;

    if (dugum->tur == AST_DEGISKEN_ATA) {
        int deger = ifadeninDegeriniHesapla(dugum->sol);
        degiskenDegeriAta(dugum->deger, deger);
    }
    else if (dugum->tur == AST_MIRR_CAGRISI) {
        mirr(dugum->deger);
    }

    astYorumla(dugum->sol);
    astYorumla(dugum->sag);
    free(dugum);
}

ASTDugum* ifadeParcala(Token* tokenler, int* pos, int tokenSayisi) {
    ASTDugum* dugum = malloc(sizeof(ASTDugum));
    if (dugum == NULL) {
        fprintf(stderr, "Bellek ayirmada hata.\n");
        exit(1);
    }

    if (tokenler[*pos].tur == TOKEN_SAYI) {
        dugum->tur = AST_SAYI;
        strcpy(dugum->deger, tokenler[*pos].deger);
        (*pos)++;
        return dugum;
    }

    if (tokenler[*pos].tur == TOKEN_KIMLIK) {
        dugum->tur = AST_SAYI;
        strcpy(dugum->deger, tokenler[*pos].deger);
        (*pos)++;
        return dugum;
    }

    return NULL;
}

ASTDugum* ifadeKomutunuParcala(Token* tokenler, int* pos, int tokenSayisi) {
    ASTDugum* dugum = malloc(sizeof(ASTDugum));
    if (dugum == NULL) {
        fprintf(stderr, "Bellek ayirmada hata.\n");
        exit(1);
    }

    if (tokenler[*pos].tur == TOKEN_KIMLIK) {
        dugum->tur = AST_DEGISKEN_ATA;
        strcpy(dugum->deger, tokenler[*pos].deger);
        (*pos)++;

        if (tokenler[*pos].tur == TOKEN_ATA) {
            (*pos)++;
            dugum->sol = ifadeParcala(tokenler, pos, tokenSayisi);
        }
    }
    else if (tokenler[*pos].tur == TOKEN_MIRR) {
        (*pos)++;
        (*pos)++;
        dugum->tur = AST_MIRR_CAGRISI;
        strcpy(dugum->deger, tokenler[*pos].deger);
        (*pos)++;
        (*pos)++;
    }

    if (tokenler[*pos].tur == TOKEN_NOKTALI_VIRGUL) {
        (*pos)++;
    }

    return dugum;
}

int main() {
    FILE *dosya = fopen("test.kedy", "r");
    if (!dosya) {
        perror("Dosya acilamadi");
        return -1;
    }

    Token tokenler[100];
    int pos = 0;
    int tokenSayisi = 0;

    while (1) {
        tokenler[tokenSayisi] = sonrakiTokenuAl(dosya);
        if (tokenler[tokenSayisi].tur == TOKEN_EOF) break;
        tokenSayisi++;
    }
    fclose(dosya);

    while (pos < tokenSayisi) {
        ASTDugum* ast = ifadeKomutunuParcala(tokenler, &pos, tokenSayisi);
        astYorumla(ast);
    }

    return 0;
}
