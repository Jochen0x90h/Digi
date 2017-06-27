const Vector2<Type> xx() const {Vector2<Type> a = {this->x, this->x}; return a;}
const Vector2<Type> yx() const {Vector2<Type> a = {this->y, this->x}; return a;}
Vector2Ref<Type> yx() {return Vector2Ref<Type>(this->y, this->x);}
const Vector2<Type> xy() const {Vector2<Type> a = {this->x, this->y}; return a;}
Vector2Ref<Type> xy() {return Vector2Ref<Type>(this->x, this->y);}
const Vector3<Type> xxx() const {Vector3<Type> a = {this->x, this->x, this->x}; return a;}
const Vector3<Type> yxx() const {Vector3<Type> a = {this->y, this->x, this->x}; return a;}
const Vector3<Type> xyx() const {Vector3<Type> a = {this->x, this->y, this->x}; return a;}
const Vector3<Type> yyx() const {Vector3<Type> a = {this->y, this->y, this->x}; return a;}
const Vector3<Type> xxy() const {Vector3<Type> a = {this->x, this->x, this->y}; return a;}
const Vector3<Type> yxy() const {Vector3<Type> a = {this->y, this->x, this->y}; return a;}
const Vector3<Type> xyy() const {Vector3<Type> a = {this->x, this->y, this->y}; return a;}
const Vector4<Type> xxxx() const {Vector4<Type> a = {this->x, this->x, this->x, this->x}; return a;}
const Vector4<Type> yxxx() const {Vector4<Type> a = {this->y, this->x, this->x, this->x}; return a;}
const Vector4<Type> xyxx() const {Vector4<Type> a = {this->x, this->y, this->x, this->x}; return a;}
const Vector4<Type> yyxx() const {Vector4<Type> a = {this->y, this->y, this->x, this->x}; return a;}
const Vector4<Type> xxyx() const {Vector4<Type> a = {this->x, this->x, this->y, this->x}; return a;}
const Vector4<Type> yxyx() const {Vector4<Type> a = {this->y, this->x, this->y, this->x}; return a;}
const Vector4<Type> xyyx() const {Vector4<Type> a = {this->x, this->y, this->y, this->x}; return a;}
const Vector4<Type> yyyx() const {Vector4<Type> a = {this->y, this->y, this->y, this->x}; return a;}
const Vector4<Type> xxxy() const {Vector4<Type> a = {this->x, this->x, this->x, this->y}; return a;}
const Vector4<Type> yxxy() const {Vector4<Type> a = {this->y, this->x, this->x, this->y}; return a;}
const Vector4<Type> xyxy() const {Vector4<Type> a = {this->x, this->y, this->x, this->y}; return a;}
const Vector4<Type> yyxy() const {Vector4<Type> a = {this->y, this->y, this->x, this->y}; return a;}
const Vector4<Type> xxyy() const {Vector4<Type> a = {this->x, this->x, this->y, this->y}; return a;}
const Vector4<Type> yxyy() const {Vector4<Type> a = {this->y, this->x, this->y, this->y}; return a;}
const Vector4<Type> xyyy() const {Vector4<Type> a = {this->x, this->y, this->y, this->y}; return a;}
