//
//  Transform.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/9/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "Transform.h"
#include "Object.h"
#include "RenderSlots.h"
#include "Quaternion.h"


using namespace std;
using namespace fx;


Transform::Transform(Object *obj): Component("Transform", obj), mRenderSlots(0)
{
  mUpdateDelegate = UpdateDelegate::Create<Transform, &Transform::update>(this);
}

Transform::~Transform()
{
}

bool Transform::setToXml(const XMLTree::Node *node)
{
  bool success = false;
  if (node)
  {
    success = true;
    for (XMLTree::const_iterator itr = node->begin(); itr != node->end(); ++itr)
    {
      pushBack(IDENITY);
      success &= back().setToXML(*itr);
    }
  }
  return success ? Component::setToXml(node) : false;
}

bool Transform::init()
{
  mRenderSlots = static_cast<RenderSlots*>(mObject->getComponentByType("RenderSlots"));
  updateMatrices();
  return mRenderSlots ? Component::init() : false;
}

void Transform::updateMatrices()
{
  mModelMatrix = mat4();
  mRotationMatrix = mat4();
  for (const_iterator itr = begin(); itr != end(); ++itr)
  {
    itr->apply(mModelMatrix);
    if (itr->type != SCALE && itr->type != TRANSLATE)
      itr->apply(mRotationMatrix);
  }
}

void Transform::update(void*)
{
  updateMatrices();
  if (mRenderSlots)
  {
    mRenderSlots->localUniforms()["Model"] = mModelMatrix;
    mRenderSlots->localUniforms()["Rotation"] = mRotationMatrix;
  }
}


bool Transform::Item::setToXML(const XMLTree::Node *node)
{
  bool success = true;
  if (node)
  {
    type = GetType(node->element());
    if (type == EULER_XYZ && node->hasAttribute("order"))
      type = GetEuler(node->attribute("order"));
    
    if (type == ANGLE_AXIS)
      data = vec4(vec3(node->contents()), node->attributeAsFloat("angle"));
    else if (type == QUATERNION)
      data = vec4(node->contents());
    else
      data = vec4(vec3(node->contents()), 0.0f);
    success = true;
  }
  return success;
}

mat4 Transform::Item::toMatrix4x4() const
{
  switch (type)
  {
    case SCALE:
      return mat4::Scale(data.xyz());
    case ANGLE_AXIS:
      return quat(data.xyz(), data.w).toMat4();
    case QUATERNION:
      return quat(data).toMat4();
    case TRANSLATE:
      return mat4::Trans3d(data.xyz());
    case EULER_XYZ:
      return mat4::RotX(data.x) * mat4::RotY(data.y) * mat4::RotZ(data.z);
    case EULER_XZY:
      return mat4::RotX(data.x) * mat4::RotZ(data.z) * mat4::RotY(data.y);
    case EULER_YXZ:
      return mat4::RotY(data.y) * mat4::RotX(data.x) * mat4::RotZ(data.z);
    case EULER_YZX:
      return mat4::RotY(data.y) * mat4::RotZ(data.z) * mat4::RotX(data.x);
    case EULER_ZXY:
      return mat4::RotZ(data.z) * mat4::RotX(data.x) * mat4::RotY(data.y);
    case EULER_ZYX:
      return mat4::RotZ(data.z) * mat4::RotY(data.y) * mat4::RotX(data.x);
    case IDENITY:
      return mat4();
  }
  return mat4();
}

Transform::TYPE Transform::GetType(const std::string &str)
{
  if (str == "Scale")
    return SCALE;
  if (str == "Axis")
    return ANGLE_AXIS;
  if (str == "Euler")
    return EULER_XYZ;
  if (str == "Quaternion")
    return QUATERNION;
  if (str == "Translate")
    return TRANSLATE;
  return IDENITY;
}
Transform::TYPE Transform::GetEuler(const std::string &str)
{
  if (str == "xyz")
    return EULER_XYZ;
  if (str == "xzy")
    return EULER_XZY;
  if (str == "yxz")
    return EULER_YXZ;
  if (str == "yzx")
    return EULER_YZX;
  if (str == "zxy")
    return EULER_ZXY;
  if (str == "zyx")
    return EULER_ZYX;
  return IDENITY;
}
