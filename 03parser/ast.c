#include "global.h"

Token *duptok(Token *token) {
    Token *t;
    
    if ((t = (Token *) malloc(sizeof(Token))) == NULL) {
        fprintf(stderr, "duptok error: can't malloc\n");
        exit(1);
    }
    t->type = token->type;
    strcpy(t->text, token->text);
    return t;
}
Anode *anode(Token *token) {
    Anode *ap;
    
    if ((ap = (Anode *) malloc(sizeof(Anode))) == NULL) {
        fprintf(stderr, "ast error: can't malloc\n");
        exit(1);
    }
    if (token == NULL)
        ap->token = NULL;
    else
        ap->token = duptok(token);
    ap->left = ap->right = NULL;
    return ap;
}
Anode *mkast(Token *tp, Anode *left, Anode *right)
{
    Anode *ap;
    
    ap = anode(tp);
    ap->left = left;
    ap->right = right;
    return ap;
}
void inorder(Anode *ap)
{
    if (ap != NULL) {
        inorder(ap->left);
        printf("%s  ", ap->token->text);
        inorder(ap->right);
    }
}

