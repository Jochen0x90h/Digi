Vector2Ref<Type> yx() {return Vector2Ref<Type>(this->yRef, this->xRef);}
Vector2Ref<Type> zx() {return Vector2Ref<Type>(this->zRef, this->xRef);}
Vector2Ref<Type> xy() {return Vector2Ref<Type>(this->xRef, this->yRef);}
Vector2Ref<Type> zy() {return Vector2Ref<Type>(this->zRef, this->yRef);}
Vector2Ref<Type> xz() {return Vector2Ref<Type>(this->xRef, this->zRef);}
Vector2Ref<Type> yz() {return Vector2Ref<Type>(this->yRef, this->zRef);}
Vector3Ref<Type> zyx() {return Vector3Ref<Type>(this->zRef, this->yRef, this->xRef);}
Vector3Ref<Type> yzx() {return Vector3Ref<Type>(this->yRef, this->zRef, this->xRef);}
Vector3Ref<Type> zxy() {return Vector3Ref<Type>(this->zRef, this->xRef, this->yRef);}
Vector3Ref<Type> xzy() {return Vector3Ref<Type>(this->xRef, this->zRef, this->yRef);}
Vector3Ref<Type> yxz() {return Vector3Ref<Type>(this->yRef, this->xRef, this->zRef);}
Vector3Ref<Type> xyz() {return Vector3Ref<Type>(this->xRef, this->yRef, this->zRef);}
