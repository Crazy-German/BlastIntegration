#pragma once
//Created by Nils.M for use at WormWorks
class Serializer;
class Serializable
{
public:
	Serializable() = default;
	virtual ~Serializable() = default;
	virtual void Serialize(Serializer& aSerializer) = 0;
private:
};