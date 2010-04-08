#include <gtest/gtest.h>
#include <typeconverter.h>

TEST(TypeConverter, get_String) {
	EXPECT_NE(TypeConverter::get("int"), (TypeConverter*) 0);
	EXPECT_NE(TypeConverter::get("string"), (TypeConverter*) 0);
	EXPECT_NE(TypeConverter::get("float"), (TypeConverter*) 0);
	EXPECT_NE(TypeConverter::get("double"), (TypeConverter*) 0);
}

TEST(TypeConverter, get_String_TypeName) {
	EXPECT_EQ(TypeConverter::get("int")->getTypeName(), "int");
	EXPECT_EQ(TypeConverter::get("double")->getTypeName(), "double");
	EXPECT_EQ(TypeConverter::get("float")->getTypeName(), "float");
	EXPECT_EQ(TypeConverter::get("string")->getTypeName(), "string");
}

TEST(TypeConverter, get_StringNot) {
	EXPECT_EQ(TypeConverter::get("notatype"), (TypeConverter*) 0);
}

TEST(TypeConverter, get_TypeInfo) {
	EXPECT_NE(TypeConverter::get(cd3::TypeInfo(typeid(int))), (TypeConverter*) 0);
	EXPECT_NE(TypeConverter::get(cd3::TypeInfo(typeid(double))), (TypeConverter*) 0);
	EXPECT_NE(TypeConverter::get(cd3::TypeInfo(typeid(float))), (TypeConverter*) 0);
	EXPECT_NE(TypeConverter::get(cd3::TypeInfo(typeid(std::string))), (TypeConverter*) 0);
}

TEST(TypeConverter, get_TypeInfo_Name) {
	EXPECT_EQ(TypeConverter::get(cd3::TypeInfo(typeid(int)))->getTypeName(), "int");
	EXPECT_EQ(TypeConverter::get(cd3::TypeInfo(typeid(double)))->getTypeName(), "double");
	EXPECT_EQ(TypeConverter::get(cd3::TypeInfo(typeid(float)))->getTypeName(), "float");
	EXPECT_EQ(TypeConverter::get(cd3::TypeInfo(typeid(std::string)))->getTypeName(), "string");
}

class NoType {

};

TEST(TypeConverter, get_TypeInfoNot) {
	EXPECT_EQ(TypeConverter::get(cd3::TypeInfo(typeid(NoType))), (TypeConverter*) 0);
}

TEST(TypeConverter, toString) {
	TypeConverter *cint = TypeConverter::get("int");
	int i = 0;
	EXPECT_EQ(cint->toString(&i), "0");
	i = 10;
	EXPECT_EQ(cint->toString(&i), "10");
	i = 2;
	EXPECT_EQ(cint->toString(&i), "2");
	i = 12345678;
	EXPECT_EQ(cint->toString(&i), "12345678");
}

TEST(TypeConverter, fromString) {
	TypeConverter *con = TypeConverter::get("int");
	int i;
	con->fromString("0", &i);
	EXPECT_EQ(i, 0);
	con->fromString("10", &i);
	EXPECT_EQ(i, 10);
	con->fromString("2", &i);
	EXPECT_EQ(i, 2);
	con->fromString("12345678", &i);
	EXPECT_EQ(i, 12345678);
}

TEST(TypeConverter, fromToStringExact) {
	TypeConverter *dcon = TypeConverter::get("double");
	double d = 2.33;
	double ddest;
	dcon->fromStringExact(dcon->toStringExact(&d), &ddest);
	EXPECT_DOUBLE_EQ(d, ddest);

	TypeConverter *icon = TypeConverter::get("int");
	int i = 10;
	int idest;
	icon->fromStringExact(icon->toStringExact(&i), &idest);
	EXPECT_DOUBLE_EQ(i, idest);
}
