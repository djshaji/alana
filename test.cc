# include <string>
# include <cstdio>

using namespace std ;

int main (int argc, char ** argv ) {
    string where = "localhost:6906:111";
    int find1 = where.find (":") ;
    int find2 = where.find (":", find1 + 1) - find1;
    
    std::string ip = where.substr (0, find1);
    std::string port = where.substr (find1 + 1, find2 - 1);
    std::string key = where.substr (find2 + find1 + 1);
    printf ("ip: %s, port: %s, key: %s\n", ip.c_str(), port.c_str (), key.c_str ());
    
} 
