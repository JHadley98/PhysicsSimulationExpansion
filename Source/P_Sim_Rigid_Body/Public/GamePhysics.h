// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <cmath>

/**
 * 
 */
class P_SIM_RIGID_BODY_API GamePhysics
	// Required for sqrt:
{
public:
	float x{ 0 };
	float y{ 0 };

	// Constructors
	GamePhysics() = default;
	GamePhysics(float newX, float newY) : x(newX), y(newY) { return; }

	// + operator. Returns the result of adding rhs to this
	GamePhysics operator + (const GamePhysics &rhs) const
	{
		return GamePhysics(x + rhs.x, y + rhs.y);
	}

	//	- operator. Returns the result of subtracting the rhs from this
	GamePhysics operator - (const GamePhysics &rhs) const
	{
		return GamePhysics(x - rhs.x, y - rhs.y);
	}

	// * operator. Returns the result of multiplying the rhs scaler to this
	GamePhysics operator * (float rhs) const
	{
		return GamePhysics(x * rhs, y * rhs);
	}

	// The sqrt makes this a relatively slow operation so it is common to
	// add a SquaredLength function that returns the squared length for use in comparisons 
	float Length() const
	{
		return sqrt(x * x + y * y);
	}

	// Normalises this vector so its length is 1
	// The resulting vector is known as a unit vector
	// Useful when you wish to just use the vector as a direction
	void NormaliseInPlace()
	{
		float len = Length();

		if (len == 0)
			return;

		y /= len;
		x /= len;
	}

	/*
		The dot product can be used to calculate the angle between 2 vectors.
		If both vectors are already unit vectors then the dot product returns the cosine of the angle between them.
		If they are not unit vectors then you must divide the result by the product of the vector lengths to get the cosine of the angle.
		Another use of the dot product is to find the distance of the point to a plane
	*/
	float Dot(const GamePhysics& other) const
	{
		return x * other.x + y * other.y;
	}

	/*
		The cross product gives a value which is 2 times the area of the triangle.
		Can also be used to find which rotation direction a vector is, relative to another (+-)
	*/
	float Cross(const GamePhysics & other)const
	{
		return x * other.y - y * other.x;
	}
	
};
