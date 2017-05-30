//TODO: try it separately
// std::string gen_random_string(const int len) {
// 	char *s;  //Don't assign NULL as it is undefined behaviour
// 	srand(time(0));
//     for (int i = 0; i < len; ++i) {
// 		int randomChar = rand()%(62);  // "0123456789" "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz"
// 		if (randomChar < 26)
// 		    s[i] = 'a' + randomChar;
// 		else if (randomChar < 52)
// 		    s[i] = 'A' + randomChar - 26;
// 		else
// 		    s[i] = '0' + randomChar - 52;
//     }
//     s[len] = 0;
//     std::string result(s, 10);
//     return result;
// }