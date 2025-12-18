
Global ::= DeclarationFunc | DeclarationVar | CondOper | CallFunc | Assign | Return | In | Out | Break | Pixel | Draw
DeclarationFunc ::= "ser" Indent '<' Indent [',' Indent] ('\n' || '(' Union ')')
DeclarationVar ::= "al" Indent ('\n' || '=' Expression ) ('\n' || ')' )
Expression ::= AddSub ['>=''>>''<=''<<''==''!=' AddSub]
AddSub ::= MulDiv ['+''-' MulDiv]
MulDiv ::= Value ['*''/' Value]
Value ::= ( '(' Expression ')' || Number || Indent )
Indent ::= ( CallFunc || Var )
CallFunc ::= [A-z'_'  A-z'_'0-9] '<' Expression [',' Expression] '>'
Var ::= [A-z'_'  A-z'_'0-9]
Number ::= ['-' 0-9 || 0-9]
Pixel ::= "pxl" '<' Expression ',' Expression ',' Expression '>' '\n'
Draw ::= "drw" '\n'
Union ::= '(' [DeclarationVar | Return | CallFunc | Assign | CondOper | Return | In | Out | Break | Draw | Pixel] ')'
In ::= "in" '<' Var '>' '\n'
Out ::= "out" '<' Expression '>' '\n'
CondOper ::= (("tr" || "ntr" '<' Expression '>') || "def" ) Union '\n'
Assign ::= Var '=' Expression '\n'
Return ::= "ret" Expression '\n'
While ::= "loop" '<' Expression '>' Union '\n'



