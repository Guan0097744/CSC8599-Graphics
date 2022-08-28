#ifndef PBRMODEL_HPP
#define PBRMODEL_HPP

#include "../Model.h"

class PBRModel: public Model
{
public:

	PBRModel(std::string id, unsigned int maxNumInstances = 1, unsigned int flags = 0)
		: Model(id, maxNumInstances, flags) {};
	
	void Init(std::string path)
	{
		LoadModel(path);
	}

};


#endif // !PBRMODEL_HPP
