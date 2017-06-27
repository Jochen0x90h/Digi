const Vector2<Type> xx() const {Vector2<Type> a = {this->x, this->x}; return a;}
const Vector2<Type> yx() const {Vector2<Type> a = {this->y, this->x}; return a;}
Vector2Ref<Type> yx() {return Vector2Ref<Type>(this->y, this->x);}
const Vector2<Type> zx() const {Vector2<Type> a = {this->z, this->x}; return a;}
Vector2Ref<Type> zx() {return Vector2Ref<Type>(this->z, this->x);}
const Vector2<Type> xy() const {Vector2<Type> a = {this->x, this->y}; return a;}
Vector2Ref<Type> xy() {return Vector2Ref<Type>(this->x, this->y);}
const Vector2<Type> yy() const {Vector2<Type> a = {this->y, this->y}; return a;}
const Vector2<Type> zy() const {Vector2<Type> a = {this->z, this->y}; return a;}
Vector2Ref<Type> zy() {return Vector2Ref<Type>(this->z, this->y);}
const Vector2<Type> xz() const {Vector2<Type> a = {this->x, this->z}; return a;}
Vector2Ref<Type> xz() {return Vector2Ref<Type>(this->x, this->z);}
const Vector2<Type> yz() const {Vector2<Type> a = {this->y, this->z}; return a;}
Vector2Ref<Type> yz() {return Vector2Ref<Type>(this->y, this->z);}
const Vector3<Type> xxx() const {Vector3<Type> a = {this->x, this->x, this->x}; return a;}
const Vector3<Type> yxx() const {Vector3<Type> a = {this->y, this->x, this->x}; return a;}
const Vector3<Type> zxx() const {Vector3<Type> a = {this->z, this->x, this->x}; return a;}
const Vector3<Type> xyx() const {Vector3<Type> a = {this->x, this->y, this->x}; return a;}
const Vector3<Type> yyx() const {Vector3<Type> a = {this->y, this->y, this->x}; return a;}
const Vector3<Type> zyx() const {Vector3<Type> a = {this->z, this->y, this->x}; return a;}
Vector3Ref<Type> zyx() {return Vector3Ref<Type>(this->z, this->y, this->x);}
const Vector3<Type> xzx() const {Vector3<Type> a = {this->x, this->z, this->x}; return a;}
const Vector3<Type> yzx() const {Vector3<Type> a = {this->y, this->z, this->x}; return a;}
Vector3Ref<Type> yzx() {return Vector3Ref<Type>(this->y, this->z, this->x);}
const Vector3<Type> zzx() const {Vector3<Type> a = {this->z, this->z, this->x}; return a;}
const Vector3<Type> xxy() const {Vector3<Type> a = {this->x, this->x, this->y}; return a;}
const Vector3<Type> yxy() const {Vector3<Type> a = {this->y, this->x, this->y}; return a;}
const Vector3<Type> zxy() const {Vector3<Type> a = {this->z, this->x, this->y}; return a;}
Vector3Ref<Type> zxy() {return Vector3Ref<Type>(this->z, this->x, this->y);}
const Vector3<Type> xyy() const {Vector3<Type> a = {this->x, this->y, this->y}; return a;}
const Vector3<Type> yyy() const {Vector3<Type> a = {this->y, this->y, this->y}; return a;}
const Vector3<Type> zyy() const {Vector3<Type> a = {this->z, this->y, this->y}; return a;}
const Vector3<Type> xzy() const {Vector3<Type> a = {this->x, this->z, this->y}; return a;}
Vector3Ref<Type> xzy() {return Vector3Ref<Type>(this->x, this->z, this->y);}
const Vector3<Type> yzy() const {Vector3<Type> a = {this->y, this->z, this->y}; return a;}
const Vector3<Type> zzy() const {Vector3<Type> a = {this->z, this->z, this->y}; return a;}
const Vector3<Type> xxz() const {Vector3<Type> a = {this->x, this->x, this->z}; return a;}
const Vector3<Type> yxz() const {Vector3<Type> a = {this->y, this->x, this->z}; return a;}
Vector3Ref<Type> yxz() {return Vector3Ref<Type>(this->y, this->x, this->z);}
const Vector3<Type> zxz() const {Vector3<Type> a = {this->z, this->x, this->z}; return a;}
const Vector3<Type> xyz() const {Vector3<Type> a = {this->x, this->y, this->z}; return a;}
Vector3Ref<Type> xyz() {return Vector3Ref<Type>(this->x, this->y, this->z);}
const Vector3<Type> yyz() const {Vector3<Type> a = {this->y, this->y, this->z}; return a;}
const Vector3<Type> zyz() const {Vector3<Type> a = {this->z, this->y, this->z}; return a;}
const Vector3<Type> xzz() const {Vector3<Type> a = {this->x, this->z, this->z}; return a;}
const Vector3<Type> yzz() const {Vector3<Type> a = {this->y, this->z, this->z}; return a;}
const Vector4<Type> xxxx() const {Vector4<Type> a = {this->x, this->x, this->x, this->x}; return a;}
const Vector4<Type> yxxx() const {Vector4<Type> a = {this->y, this->x, this->x, this->x}; return a;}
const Vector4<Type> zxxx() const {Vector4<Type> a = {this->z, this->x, this->x, this->x}; return a;}
const Vector4<Type> xyxx() const {Vector4<Type> a = {this->x, this->y, this->x, this->x}; return a;}
const Vector4<Type> yyxx() const {Vector4<Type> a = {this->y, this->y, this->x, this->x}; return a;}
const Vector4<Type> zyxx() const {Vector4<Type> a = {this->z, this->y, this->x, this->x}; return a;}
const Vector4<Type> xzxx() const {Vector4<Type> a = {this->x, this->z, this->x, this->x}; return a;}
const Vector4<Type> yzxx() const {Vector4<Type> a = {this->y, this->z, this->x, this->x}; return a;}
const Vector4<Type> zzxx() const {Vector4<Type> a = {this->z, this->z, this->x, this->x}; return a;}
const Vector4<Type> xxyx() const {Vector4<Type> a = {this->x, this->x, this->y, this->x}; return a;}
const Vector4<Type> yxyx() const {Vector4<Type> a = {this->y, this->x, this->y, this->x}; return a;}
const Vector4<Type> zxyx() const {Vector4<Type> a = {this->z, this->x, this->y, this->x}; return a;}
const Vector4<Type> xyyx() const {Vector4<Type> a = {this->x, this->y, this->y, this->x}; return a;}
const Vector4<Type> yyyx() const {Vector4<Type> a = {this->y, this->y, this->y, this->x}; return a;}
const Vector4<Type> zyyx() const {Vector4<Type> a = {this->z, this->y, this->y, this->x}; return a;}
const Vector4<Type> xzyx() const {Vector4<Type> a = {this->x, this->z, this->y, this->x}; return a;}
const Vector4<Type> yzyx() const {Vector4<Type> a = {this->y, this->z, this->y, this->x}; return a;}
const Vector4<Type> zzyx() const {Vector4<Type> a = {this->z, this->z, this->y, this->x}; return a;}
const Vector4<Type> xxzx() const {Vector4<Type> a = {this->x, this->x, this->z, this->x}; return a;}
const Vector4<Type> yxzx() const {Vector4<Type> a = {this->y, this->x, this->z, this->x}; return a;}
const Vector4<Type> zxzx() const {Vector4<Type> a = {this->z, this->x, this->z, this->x}; return a;}
const Vector4<Type> xyzx() const {Vector4<Type> a = {this->x, this->y, this->z, this->x}; return a;}
const Vector4<Type> yyzx() const {Vector4<Type> a = {this->y, this->y, this->z, this->x}; return a;}
const Vector4<Type> zyzx() const {Vector4<Type> a = {this->z, this->y, this->z, this->x}; return a;}
const Vector4<Type> xzzx() const {Vector4<Type> a = {this->x, this->z, this->z, this->x}; return a;}
const Vector4<Type> yzzx() const {Vector4<Type> a = {this->y, this->z, this->z, this->x}; return a;}
const Vector4<Type> zzzx() const {Vector4<Type> a = {this->z, this->z, this->z, this->x}; return a;}
const Vector4<Type> xxxy() const {Vector4<Type> a = {this->x, this->x, this->x, this->y}; return a;}
const Vector4<Type> yxxy() const {Vector4<Type> a = {this->y, this->x, this->x, this->y}; return a;}
const Vector4<Type> zxxy() const {Vector4<Type> a = {this->z, this->x, this->x, this->y}; return a;}
const Vector4<Type> xyxy() const {Vector4<Type> a = {this->x, this->y, this->x, this->y}; return a;}
const Vector4<Type> yyxy() const {Vector4<Type> a = {this->y, this->y, this->x, this->y}; return a;}
const Vector4<Type> zyxy() const {Vector4<Type> a = {this->z, this->y, this->x, this->y}; return a;}
const Vector4<Type> xzxy() const {Vector4<Type> a = {this->x, this->z, this->x, this->y}; return a;}
const Vector4<Type> yzxy() const {Vector4<Type> a = {this->y, this->z, this->x, this->y}; return a;}
const Vector4<Type> zzxy() const {Vector4<Type> a = {this->z, this->z, this->x, this->y}; return a;}
const Vector4<Type> xxyy() const {Vector4<Type> a = {this->x, this->x, this->y, this->y}; return a;}
const Vector4<Type> yxyy() const {Vector4<Type> a = {this->y, this->x, this->y, this->y}; return a;}
const Vector4<Type> zxyy() const {Vector4<Type> a = {this->z, this->x, this->y, this->y}; return a;}
const Vector4<Type> xyyy() const {Vector4<Type> a = {this->x, this->y, this->y, this->y}; return a;}
const Vector4<Type> yyyy() const {Vector4<Type> a = {this->y, this->y, this->y, this->y}; return a;}
const Vector4<Type> zyyy() const {Vector4<Type> a = {this->z, this->y, this->y, this->y}; return a;}
const Vector4<Type> xzyy() const {Vector4<Type> a = {this->x, this->z, this->y, this->y}; return a;}
const Vector4<Type> yzyy() const {Vector4<Type> a = {this->y, this->z, this->y, this->y}; return a;}
const Vector4<Type> zzyy() const {Vector4<Type> a = {this->z, this->z, this->y, this->y}; return a;}
const Vector4<Type> xxzy() const {Vector4<Type> a = {this->x, this->x, this->z, this->y}; return a;}
const Vector4<Type> yxzy() const {Vector4<Type> a = {this->y, this->x, this->z, this->y}; return a;}
const Vector4<Type> zxzy() const {Vector4<Type> a = {this->z, this->x, this->z, this->y}; return a;}
const Vector4<Type> xyzy() const {Vector4<Type> a = {this->x, this->y, this->z, this->y}; return a;}
const Vector4<Type> yyzy() const {Vector4<Type> a = {this->y, this->y, this->z, this->y}; return a;}
const Vector4<Type> zyzy() const {Vector4<Type> a = {this->z, this->y, this->z, this->y}; return a;}
const Vector4<Type> xzzy() const {Vector4<Type> a = {this->x, this->z, this->z, this->y}; return a;}
const Vector4<Type> yzzy() const {Vector4<Type> a = {this->y, this->z, this->z, this->y}; return a;}
const Vector4<Type> zzzy() const {Vector4<Type> a = {this->z, this->z, this->z, this->y}; return a;}
const Vector4<Type> xxxz() const {Vector4<Type> a = {this->x, this->x, this->x, this->z}; return a;}
const Vector4<Type> yxxz() const {Vector4<Type> a = {this->y, this->x, this->x, this->z}; return a;}
const Vector4<Type> zxxz() const {Vector4<Type> a = {this->z, this->x, this->x, this->z}; return a;}
const Vector4<Type> xyxz() const {Vector4<Type> a = {this->x, this->y, this->x, this->z}; return a;}
const Vector4<Type> yyxz() const {Vector4<Type> a = {this->y, this->y, this->x, this->z}; return a;}
const Vector4<Type> zyxz() const {Vector4<Type> a = {this->z, this->y, this->x, this->z}; return a;}
const Vector4<Type> xzxz() const {Vector4<Type> a = {this->x, this->z, this->x, this->z}; return a;}
const Vector4<Type> yzxz() const {Vector4<Type> a = {this->y, this->z, this->x, this->z}; return a;}
const Vector4<Type> zzxz() const {Vector4<Type> a = {this->z, this->z, this->x, this->z}; return a;}
const Vector4<Type> xxyz() const {Vector4<Type> a = {this->x, this->x, this->y, this->z}; return a;}
const Vector4<Type> yxyz() const {Vector4<Type> a = {this->y, this->x, this->y, this->z}; return a;}
const Vector4<Type> zxyz() const {Vector4<Type> a = {this->z, this->x, this->y, this->z}; return a;}
const Vector4<Type> xyyz() const {Vector4<Type> a = {this->x, this->y, this->y, this->z}; return a;}
const Vector4<Type> yyyz() const {Vector4<Type> a = {this->y, this->y, this->y, this->z}; return a;}
const Vector4<Type> zyyz() const {Vector4<Type> a = {this->z, this->y, this->y, this->z}; return a;}
const Vector4<Type> xzyz() const {Vector4<Type> a = {this->x, this->z, this->y, this->z}; return a;}
const Vector4<Type> yzyz() const {Vector4<Type> a = {this->y, this->z, this->y, this->z}; return a;}
const Vector4<Type> zzyz() const {Vector4<Type> a = {this->z, this->z, this->y, this->z}; return a;}
const Vector4<Type> xxzz() const {Vector4<Type> a = {this->x, this->x, this->z, this->z}; return a;}
const Vector4<Type> yxzz() const {Vector4<Type> a = {this->y, this->x, this->z, this->z}; return a;}
const Vector4<Type> zxzz() const {Vector4<Type> a = {this->z, this->x, this->z, this->z}; return a;}
const Vector4<Type> xyzz() const {Vector4<Type> a = {this->x, this->y, this->z, this->z}; return a;}
const Vector4<Type> yyzz() const {Vector4<Type> a = {this->y, this->y, this->z, this->z}; return a;}
const Vector4<Type> zyzz() const {Vector4<Type> a = {this->z, this->y, this->z, this->z}; return a;}
const Vector4<Type> xzzz() const {Vector4<Type> a = {this->x, this->z, this->z, this->z}; return a;}
const Vector4<Type> yzzz() const {Vector4<Type> a = {this->y, this->z, this->z, this->z}; return a;}
