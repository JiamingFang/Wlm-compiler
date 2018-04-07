#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <map>

// This is kept as a struct for simplicity in this tutorial.
// You might want to reorganize this when approaching it in your
// assignment.
using namespace std;
map <string, int> symTable;
map <string, bool> funcTable;
int adress = 1;
vector<vector<string>> allcode;
int countId = 0;
int countId2 = 0;

struct Tree {
  string rule;
  vector<unique_ptr<Tree>> children;
  vector<vector<string>> code;

  Tree(string rule, vector<unique_ptr<Tree>> &&children):
    rule{move(rule)}, children{move(children)},
    code{this->children.size() + 1} {}
};


void makeSym(const Tree &tree){

  if(tree.rule == "dcl INT ID"){
    string id = tree.children[1]->rule.substr(3,tree.children[1]->rule.length()-3);
    if(symTable.find(id) == symTable.end()){
      symTable[id] = adress;
      adress++;
    }
    else{
      cerr << "ERROR: duplicate" << endl;
    }
  }
  for (auto &child : tree.children) makeSym(*child);
}

unique_ptr<Tree> readTree() {
  string line;
  vector<unique_ptr<Tree>> children;
  getline(cin, line);

  if (line == "procedures procedure procedures"
      || line == "dcl INT ID" || line == "statements statements statement"
      || line == "statement pcall SEMI") {
    children.push_back(readTree());
    children.push_back(readTree());
  } else if(line == "expr term" || line == "term factor" || line == "factor NUM" || line == "factor ID" || line == "factor pcall" || line == "params dcl"){
    children.push_back(readTree());
  } else if (line == "params dcl COMMA dcl" || line == "start BOF procedures EOF" 
    || line == "expr expr PLUS term" || line == "expr expr MINUS term"
    || line == "term term STAR factor" || line == "term term SLASH factor"
    || line == "term term PCT factor" || line == "factor LPAREN expr RPAREN"
    || line == "pcall ID LPAREN RPAREN" || line == "test expr LT expr"
    || line == "test expr EQ expr" || line == "test expr NE expr"
    || line == "test expr LE expr" || line == "test expr GT expr"
    || line == "test expr GE expr"){
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
  } else if(line == "pcall ID LPAREN expr RPAREN" || line == "statement ID BECOMES expr SEMI"){
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
  } else if(line == "dcls dcls dcl BECOMES NUM SEMI"){
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
  }
  else if(line == "pcall ID LPAREN expr COMMA expr RPAREN"){
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
  }
  else if(line == "statement WHILE LPAREN test RPAREN LBRACE statements RBRACE"){
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
  }
  else if(line == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE"){
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
  }
  else if(line == "statement IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE"){
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
    children.push_back(readTree());
  }

  return make_unique<Tree>(line, move(children));
}

vector<string> push(int reg) {
  return {"sw $" + to_string(reg) + ", -4($30)", "sub $30, $30, $4"};
}

vector<string> pop(int reg) {
  return {"add $30, $30, $4", "lw $" + to_string(reg) + ", -4($30)"};
}

void factor_code(Tree &tree);
void term_code(Tree &tree);
void expr_code(Tree &tree);
void dcls_code(Tree &tree);
void statement_code(Tree &tree);
void statements_code(Tree &tree);
void test_code(Tree &tree);
void procedures_code(Tree &tree);
void procedure_code(Tree &tree);

void test_code(Tree &tree){
  if(tree.rule == "test expr LT expr"){
    expr_code(*tree.children[0]);
    vector<string> temp;
    //push 3
    auto store2 = push(3);
    copy(store2.begin(), store2.end(), back_inserter(temp));
    allcode.push_back(temp);
    expr_code(*tree.children[2]);
    //pop 5
    vector<string> temp2;
    auto load = pop(5);
    copy(load.begin(), load.end(), back_inserter(temp2));
    allcode.push_back(temp2);
    allcode.push_back({"slt $3, $5, $3"});
  }
  else if(tree.rule == "test expr EQ expr"){
    expr_code(*tree.children[0]);
    vector<string> temp;
    //push 3
    auto store2 = push(3);
    copy(store2.begin(), store2.end(), back_inserter(temp));
    allcode.push_back(temp);
    expr_code(*tree.children[2]);
    //pop 5
    vector<string> temp2;
    auto load = pop(5);
    copy(load.begin(), load.end(), back_inserter(temp2));
    allcode.push_back(temp2);
    allcode.push_back({"sub $3, $5, $3"});
    allcode.push_back({"beq $3, $0, 1"});
    allcode.push_back({"add $3, $11, $0"});
    allcode.push_back({"slt $3, $3, $11"});
  }
  else if(tree.rule == "test expr NE expr"){
    expr_code(*tree.children[0]);
    vector<string> temp;
    //push 3
    auto store2 = push(3);
    copy(store2.begin(), store2.end(), back_inserter(temp));
    allcode.push_back(temp);
    expr_code(*tree.children[2]);
    //pop 5
    vector<string> temp2;
    auto load = pop(5);
    copy(load.begin(), load.end(), back_inserter(temp2));
    allcode.push_back(temp2);
    allcode.push_back({"bne $3, $5, 2"});
    allcode.push_back({"add $3, $0, $0"});
    allcode.push_back({"beq $0, $0, 1"});
    allcode.push_back({"add $3, $11, $0"});
  }
  else if(tree.rule == "test expr LE expr"){
    expr_code(*tree.children[0]);
    vector<string> temp;
    //push 3
    auto store2 = push(3);
    copy(store2.begin(), store2.end(), back_inserter(temp));
    allcode.push_back(temp);
    expr_code(*tree.children[2]);
    //pop 5
    vector<string> temp2;
    auto load = pop(5);
    copy(load.begin(), load.end(), back_inserter(temp2));
    allcode.push_back(temp2);
    allcode.push_back({"add $3, $3, $11"});
    allcode.push_back({"slt $3, $5, $3"});
  }
  else if(tree.rule == "test expr GT expr"){
    expr_code(*tree.children[0]);
    vector<string> temp;
    //push 3
    auto store2 = push(3);
    copy(store2.begin(), store2.end(), back_inserter(temp));
    allcode.push_back(temp);
    expr_code(*tree.children[2]);
    //pop 5
    vector<string> temp2;
    auto load = pop(5);
    copy(load.begin(), load.end(), back_inserter(temp2));
    allcode.push_back(temp2);
    allcode.push_back({"slt $3, $3, $5"});
  }
  else if(tree.rule == "test expr GE expr"){
    expr_code(*tree.children[0]);
    vector<string> temp;
    //push 3
    auto store2 = push(3);
    copy(store2.begin(), store2.end(), back_inserter(temp));
    allcode.push_back(temp);
    expr_code(*tree.children[2]);
    //pop 5
    vector<string> temp2;
    auto load = pop(5);
    copy(load.begin(), load.end(), back_inserter(temp2));
    allcode.push_back(temp2);
    allcode.push_back({"add $5, $5, $11"});
    allcode.push_back({"slt $3, $3, $5"});
  }
}

void pcall_code(Tree &tree){
    //check if function defined
  string funcName = tree.children[0]->rule.substr(3,tree.children[0]->rule.length()-3);
  if(funcTable.find(funcName) == funcTable.end()){
    cerr << "ERROR: undefined function" << endl;
  } 
  if(tree.rule == "pcall ID LPAREN RPAREN"){
    if(tree.children[0]->rule == "ID getchar"){
      allcode.push_back({"lis $3"});
      allcode.push_back({".word 0xffff0004"});
      allcode.push_back({"lw $3, 0($3)"});
    }
    else{
      string functionName2 = ".word " + tree.children[0]->rule.substr(3,tree.children[0]->rule.length()-3);
      allcode.push_back({"lis $31"});
      allcode.push_back({functionName2});
      allcode.push_back({"jalr $31"});
    }
  }
  else if(tree.rule == "pcall ID LPAREN expr RPAREN"){
    expr_code(*tree.children[2]);
    if(tree.children[0]->rule == "ID putchar"){
    //push 3
      expr_code(*tree.children[2]);
      allcode.push_back({"add $5, $3, $0"});

      allcode.push_back({"lis $3"});
      allcode.push_back({".word 0xffff000c"});
      allcode.push_back({"sw $5, 0($3)"});
    }
    else{
      string functionName2 = ".word " + tree.children[0]->rule.substr(3,tree.children[0]->rule.length()-3);
      expr_code(*tree.children[2]);
      allcode.push_back({"add $1, $3, $0"});
      allcode.push_back({"lis $31"});
      allcode.push_back({functionName2});
      allcode.push_back({"jalr $31"});
    }
    
  }
  else if(tree.rule == "pcall ID LPAREN expr COMMA expr RPAREN"){
    string functionName2 = ".word " + tree.children[0]->rule.substr(3,tree.children[0]->rule.length()-3);
    expr_code(*tree.children[2]);
    allcode.push_back({"add $1, $3, $0"});
    expr_code(*tree.children[4]);
    allcode.push_back({"add $2, $3, $0"});
    allcode.push_back({"lis $31"});
    allcode.push_back({functionName2});
    allcode.push_back({"jalr $31"});
  }
}

void statement_code(Tree &tree){
  if(tree.rule == "statement pcall SEMI"){
    pcall_code(*tree.children[0]);
  }
  else if(tree.rule == "statement ID BECOMES expr SEMI"){
    expr_code(*tree.children[2]);
    string id = tree.children[0]->rule.substr(3,tree.children[0]->rule.length()-3);
    try{      
      int adTemp = symTable.at(id);
      adTemp*=-4;
      string var = "sw $3, " + to_string(adTemp) + "($29)";
      allcode.push_back({var});
    }
    catch(...){
      cerr << "ERROR: undefined ID" << endl;
    }
  }
  else if(tree.rule == "statement WHILE LPAREN test RPAREN LBRACE statements RBRACE"){
    string name = "loop" + to_string(countId) + ":";
    int tempCount = countId;
    countId++;
    allcode.push_back({name});
    test_code(*tree.children[2]);
    string name2 = "beq $3, $0, endloop" + to_string(tempCount);
    allcode.push_back({name2});
    statements_code(*tree.children[5]);
    string name3 = "beq $0, $0, loop" + to_string(tempCount);
    allcode.push_back({name3});
    string name4 = "endloop" + to_string(tempCount) + ":";
    allcode.push_back({name4});
  }
  else if(tree.rule == "statement IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE"){
    int temp = countId2;
    countId2++;
    test_code(*tree.children[2]);
    allcode.push_back({"beq $3, $0, else"+ to_string(temp)});
    statements_code(*tree.children[5]);
    allcode.push_back({"beq $0, $0, endif"+ to_string(temp)});
    allcode.push_back({"else"+ to_string(temp) + ":"});
    statements_code(*tree.children[9]);
    allcode.push_back({"endif"+ to_string(temp) + ":"});
  }
  
}

void statements_code(Tree &tree){
  if(tree.rule == "statements statements statement"){
    statements_code(*tree.children[0]);
    statement_code(*tree.children[1]);
  }
}

void factor_code(Tree &tree){
  if(tree.rule == "factor NUM"){
    allcode.push_back({"lis $3"});
    string num = tree.children[0]->rule.substr(4,tree.children[0]->rule.length()-4);
    try{
      int num2 = stoi(num);
      allcode.push_back({".word " + to_string(num2)});
    }
    catch(...){
      cerr << "ERROR: out of range" << endl;
    }
  }
  else if(tree.rule == "factor ID"){
    string id = tree.children[0]->rule.substr(3,tree.children[0]->rule.length()-3);
    try{      
      int adTemp = symTable.at(id);
      adTemp*=-4;
      string var = "lw $3, " + to_string(adTemp) + "($29)";
      allcode.push_back({var});
    }
    catch(...){
      cerr << "ERROR: undefined ID" << endl;
    }
  }
  else if(tree.rule == "factor LPAREN expr RPAREN"){
    expr_code(*tree.children[1]);
  }
  else if(tree.rule == "factor pcall"){
    pcall_code(*tree.children[0]);
  }
}

void term_code(Tree &tree){
  if(tree.rule == "term factor"){
    factor_code(*tree.children[0]);
  }
  else if(tree.rule == "term term STAR factor"){
    term_code(*tree.children[0]);
    //push
    vector<string> temp;
    auto store = push(3);
    copy(store.begin(), store.end(), back_inserter(temp));
    allcode.push_back(temp);

    factor_code(*tree.children[2]);
    //pop
    vector<string> temp2;
    auto load = pop(5);
    copy(load.begin(), load.end(), back_inserter(temp2));
    allcode.push_back(temp2);

    allcode.push_back({"mult $3, $5"});
    allcode.push_back({"mflo $3"});
  }
  else if(tree.rule == "term term SLASH factor"){
    term_code(*tree.children[0]);
    //push
    vector<string> temp;
    auto store = push(3);
    copy(store.begin(), store.end(), back_inserter(temp));
    allcode.push_back(temp);

    factor_code(*tree.children[2]);
    //pop
    vector<string> temp2;
    auto load = pop(5);
    copy(load.begin(), load.end(), back_inserter(temp2));
    allcode.push_back(temp2);

    allcode.push_back({"div $5, $3"});
    allcode.push_back({"mflo $3"});
  }
  else if(tree.rule == "term term PCT factor"){
    term_code(*tree.children[0]);
    //push
    vector<string> temp;
    auto store = push(3);
    copy(store.begin(), store.end(), back_inserter(temp));
    allcode.push_back(temp);

    factor_code(*tree.children[2]);
    //pop
    vector<string> temp2;
    auto load = pop(5);
    copy(load.begin(), load.end(), back_inserter(temp2));
    allcode.push_back(temp2);

    allcode.push_back({"div $5, $3"});
    allcode.push_back({"mfhi $3"});
  }
}

void dcls_code(Tree &tree){
  if(tree.rule == "dcls dcls dcl BECOMES NUM SEMI"){
    dcls_code(*tree.children[0]);
    allcode.push_back({"lis $3"});
    string num = tree.children[3]->rule.substr(4,tree.children[3]->rule.length()-4);
    try{
      int num2 = stoi(num);
      allcode.push_back({".word " + to_string(num2)});
    }
    catch(...){
      cerr << "ERROR: out of range" << endl;
    }
    vector<string> temp3;
    auto store = push(3);
    copy(store.begin(), store.end(), back_inserter(temp3));
    allcode.push_back(temp3);
  }
}


void expr_code(Tree &tree){
  if(tree.rule == "expr term"){
    term_code(*tree.children[0]);
  }
  else if(tree.rule == "expr expr PLUS term"){
    expr_code(*tree.children[0]);
    //push
    vector<string> temp;
    auto store = push(3);
    copy(store.begin(), store.end(), back_inserter(temp));
    allcode.push_back(temp);

    term_code(*tree.children[2]);
    //pop
    vector<string> temp2;
    auto load = pop(5);
    copy(load.begin(), load.end(), back_inserter(temp2));
    allcode.push_back(temp2);

    allcode.push_back({"add $3, $3, $5"});
  }
  else if(tree.rule == "expr expr MINUS term"){
    expr_code(*tree.children[0]);
    //push
    vector<string> temp;
    auto store = push(3);
    copy(store.begin(), store.end(), back_inserter(temp));
    allcode.push_back(temp);

    term_code(*tree.children[2]);
    //pop
    vector<string> temp2;
    auto load = pop(5);
    copy(load.begin(), load.end(), back_inserter(temp2));
    allcode.push_back(temp2);

    allcode.push_back({"sub $3, $5, $3"});
  }
}

void params_code(Tree &tree){
  if(tree.rule == "params dcl COMMA dcl"){
    vector<string> var1;
    vector<string> var2;
    auto store5 = push(1);
    copy(store5.begin(), store5.end(), back_inserter(var1));
    allcode.push_back(var1);
    auto store6 = push(2);
    copy(store6.begin(), store6.end(), back_inserter(var2));
    allcode.push_back(var2);
  }
  else if(tree.rule == "params dcl"){
    vector<string> var1;
    auto store5 = push(1);
    copy(store5.begin(), store5.end(), back_inserter(var1));
    allcode.push_back(var1);
  }
}

void procedure_code(Tree &tree){
  if(tree.rule == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE"){
    makeSym(tree);

    string funcName = tree.children[1]->rule.substr(3,tree.children[1]->rule.length()-3) + ":";
    allcode.push_back({funcName});

    //push 31,29,5
    vector<string> temp31;
    vector<string> temp29;
    vector<string> temp5;
    auto store2 = push(31);
    copy(store2.begin(), store2.end(), back_inserter(temp31));
    allcode.push_back(temp31);

    auto store3 = push(29);
    copy(store3.begin(), store3.end(), back_inserter(temp29));
    allcode.push_back(temp29);

    auto store4 = push(5);
    copy(store4.begin(), store4.end(), back_inserter(temp5));
    allcode.push_back(temp5);

    allcode.push_back({"add $29, $30, $0"});
    params_code(*tree.children[3]);
    dcls_code(*tree.children[6]);
    statements_code(*tree.children[7]);
    expr_code(*tree.children[9]);
    symTable.clear();
    adress = 1;
  }
}

void procedures_code(Tree &tree){
  if(tree.rule == "procedures procedure procedures"){
    procedure_code(*tree.children[0]);
    allcode.push_back({"add $30, $29, $0"});
    allcode.push_back({"add $30, $30, $4"});
    allcode.push_back({"lw $5, -4($30)"});
    allcode.push_back({"add $30, $30, $4"});
    allcode.push_back({"lw $29, -4($30)"});
    allcode.push_back({"add $30, $30, $4"});
    allcode.push_back({"lw $31, -4($30)"});
    allcode.push_back({"jr $31"});
    procedures_code(*tree.children[1]);
  }
}

void generateCode(Tree &tree) {
  if (tree.rule == "start BOF procedures EOF") {
    //pre
    vector<string> temp;
    allcode.push_back({"lis $11"});
    allcode.push_back({".word 1"});
    allcode.push_back({"lis $4"});
    allcode.push_back({".word 4"});
    auto store = push(31);
    copy(store.begin(), store.end(), back_inserter(temp));
    allcode.push_back(temp);
    allcode.push_back({"lis $31"});
    allcode.push_back({".word wain"});
    allcode.push_back({"jalr $31"});
//post
    vector<string> temp2;
    auto load = pop(31);
    copy(load.begin(), load.end(), back_inserter(temp2));
    allcode.push_back(temp2);
    allcode.push_back({"jr $31"});
    //call procedures
    procedures_code(*tree.children[1]);
    
  }
}

void printCode(const Tree &tree) {
  for(auto &outer : allcode){
    for(auto &inner : outer){
      cout << inner << endl;
    }
  }
}

void funcTab(const Tree &tree){
  if(tree.rule == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE"){
    string functionName = tree.children[1]->rule.substr(3,tree.children[1]->rule.length()-3);
    if(funcTable.find(functionName) == funcTable.end()){
      funcTable[functionName] = true;
    }
    else{
      cerr << "ERROR: duplicate" << endl;
    }
  }
  for (auto &child : tree.children) funcTab(*child);
}

void funcTab2(const Tree &tree){
  if(tree.rule == "pcall ID LPAREN RPAREN" || tree.rule == "pcall ID LPAREN expr RPAREN" || tree.rule == "pcall ID LPAREN expr COMMA expr RPAREN"){
    string funcName = tree.children[0]->rule.substr(3,tree.children[0]->rule.length()-3);
    if(funcTable.find(funcName) == funcTable.end()){
      cerr << "ERROR: undefined function" << endl;
    } 
  }
  for (auto &child : tree.children) funcTab2(*child);
}

int main() {
  funcTable["getchar"] = true;
  funcTable["putchar"] = true;
  unique_ptr<Tree> tree = readTree();
  funcTab(*tree);
  funcTab2(*tree);
  generateCode(*tree);
  printCode(*tree);
}