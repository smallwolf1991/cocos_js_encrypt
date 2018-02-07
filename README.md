### 方法一
        
//D:\CocosCreator\resources\cocos2d-x\cocos\platform\CCImage.h
//initWithImageFile 上面加入下面代码
//引用头文件 
#include "base/CCData.h"
void Image_Decrypt(Data * data);

//D:\CocosCreator\resources\cocos2d-x\cocos\platform\CCImage.cpp
//initWithImageFile 上面加入下面代码

//引用头文件 
#include "external\sources\xxtea\xxtea.h"

#define DECRYPT_SIGN "sw"
#define DECRYPT_KEY "aaaaaa"

void Image::Image_Decrypt(Data *data)
{
	const char* key = DECRYPT_KEY;
	const char* sign = DECRYPT_SIGN;
	unsigned char* dataBytes = data->getBytes();
	ssize_t dataLen = data->getSize();
	ssize_t signLen = strlen(sign);
	ssize_t keyLen = strlen(key);

	if (strncmp(sign, (const char*)dataBytes, signLen) != 0)
	{
		return;
	}

	xxtea_long retLen = 0;
	unsigned char* retData = xxtea_decrypt(dataBytes + signLen, dataLen - signLen, (unsigned char*)key, keyLen, &retLen);
	data->fastSet(retData, retLen);
}

bool Image::initWithImageFile(const std::string& path)
{
    bool ret = false;
    _filePath = FileUtils::getInstance()->fullPathForFilename(path);

    Data data = FileUtils::getInstance()->getDataFromFile(_filePath);

    if (!data.isNull())
    {           
		//关闭文件
		Image_Decrypt(&data);
        ret = initWithImageData(data.getBytes(), data.getSize());
    }

    return ret;
}
    
### 方法二
    
在D:\CocosCreator\resources\cocos2d-x\cocos\platform\CCFileUtils.cpp文件加入下面代码

#include "external\sources\xxtea\xxtea.h" // 需要引用头文件

bool FileUtils::Data_Decrypt(Data *data) // 放到FileUtils的类中
{
	const char* key = DECRYPT_KEY;
	const char* sign = DECRYPT_SIGN;
	unsigned char* dataBytes = data->getBytes();
	ssize_t dataLen = data->getSize();
	ssize_t signLen = strlen(sign);
	ssize_t keyLen = strlen(key);

	if (strncmp(sign, (const char*)dataBytes, signLen) != 0)
	{
		return false;
	}

	xxtea_long retLen = 0;
	unsigned char* retData = xxtea_decrypt(dataBytes + signLen, dataLen - signLen, (unsigned char*)key, keyLen, &retLen);
	data->fastSet(retData, retLen);
	return true;
}
在D:\CocosCreator\resources\cocos2d-x\cocos\platform\CCFileUtils.h加入声明，并添加预定义

#define DECRYPT_SIGN "BGtISRNh8wEX5sRp"
#define DECRYPT_KEY "U4I5yavAEq12Na4qtgDt"


bool FileUtils::Data_Decrypt(Data *data);// 这个需要加到FileUtils类的公共方法区域



解密的方法已经好了，下面需要在方法一种的initWithImageFile中的相同位置加入类似代码，注意区别（步骤一的其他操作都不需要，只需要加入下面代码）：

FileUtils::getInstance()->Data_Decrypt(&data);

这就实现图片的解密。

然后可以在D:\CocosCreator\resources\cocos2d-x\cocos\scripting\js-bindings\manual\jsb_global.cpp中onGetDataFromFile和onGetStringFromFile修改代码实现js脚本的解密，这样就可以把cocos自带的js也实现加密
    