#ifndef TYPEID_H
#define TYPEID_H

#include <string.h>
#include <stdlib.h>
#include <typeinfo>

namespace cd3 {

class TypeInfo {
public:
	TypeInfo();
	TypeInfo(const std::type_info &info);
	//TypeInfo(const TypeInfo &other);
	//~TypeInfo();
	std::string type_name;
};


inline bool operator==(const TypeInfo &lhs, const TypeInfo &rhs) {
	return lhs.type_name.compare(rhs.type_name.c_str()) == 0;
	return true;
}

inline bool operator!=(const TypeInfo &lhs, const TypeInfo &rhs) {
	return !(rhs == lhs);
	return true;
}

inline bool operator < (const TypeInfo &lhs, const TypeInfo &rhs) {
	return lhs.type_name.compare(rhs.type_name.c_str()) < 0;
}

inline TypeInfo::TypeInfo() {
	class Nil{};
	type_name = typeid(Nil).name();
}

inline TypeInfo::TypeInfo(const std::type_info &info) {
	type_name = info.name();
}

}
#endif // TYPEID_H
