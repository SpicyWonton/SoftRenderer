#include "light.h"
#include "math_util.h"

// normal distribution function
double Light::NDF(const Vector4d &n, const Vector4d &h, double roughness) {
  double a = roughness * roughness;
  double a2 = a * a;
  double n_dot_h = std::max(n.Dot(h), 0.0);
  double n_dot_h2 = n_dot_h * n_dot_h;

  double nom = a2;
  double denom = (n_dot_h2 * (a2 - 1.0) + 1.0);
  denom = kPI * denom * denom;

  return nom / denom;
}
// fresnel function
Vector4d Light::FE(const Vector4d &f0, const Vector4d &h, const Vector4d &v) {
  double h_dot_v = std::max(h.Dot(v), 0.0);
  Vector4d vec(1.0, 1.0, 1.0, 0.0);

  return f0 + (vec - f0) * std::pow(1.0 - h_dot_v, 5);
}
// geometry function
double Light::GE(const Vector4d &n, const Vector4d &v, const Vector4d &l, double roughness) {
  double n_dot_v = std::max(n.Dot(v), 0.0);
  double n_dot_l = std::max(n.Dot(l), 0.0);
  double ggx2 = GESub(n_dot_v, roughness);
  double ggx1 = GESub(n_dot_l, roughness);

  return ggx1 * ggx2;
}

double Light::GESub(double n_dot_v, double roughness) {
  double r = roughness + 1.0;
  double k = (r * r) / 8.0;

  double nom = n_dot_v;
  double denom = n_dot_v * (1.0 - k) + k;

  return nom / denom;
}

Vector4d DirectionLight::Lighting(const Vector4d &normal,
								  const Vector4d &pos,
								  const Vector4d &view_pos,
								  const Vector4d &albedo,
								  bool occlude) {
  if (occlude)
	return ambient_ * albedo;
  double cos_theta;
  Vector4d ambient, diffuse, specular, view_dir, reflect_dir;
  // calculate ambient coefficient
  ambient = ambient_;
  // calculate diffuse coefficient
  cos_theta = std::max(0.0, light_dir_.Dot(normal));
  diffuse = diffuse_ * cos_theta;
  // calculate specular coefficient
  view_dir = (view_pos - pos).Normalize();
  reflect_dir = Reflect(light_dir_, normal).Normalize();
  cos_theta = std::max(0.0, view_dir.Dot(reflect_dir));
  specular = specular_ * std::pow(cos_theta, 32);
  // merge together
  return (ambient + diffuse + specular) * albedo;
}

Vector4d PointLight::Lighting(const Vector4d &normal,
							  const Vector4d &pos,
							  const Vector4d &view_pos,
							  const Vector4d &albedo,
							  bool occlude) {
  double cos_theta, distance, attenuation;
  Vector4d ambient, diffuse, specular, view_dir, reflect_dir, light_dir;
  // calculate attenuation
  light_dir = light_pos_ - pos;
  distance = light_dir.Norm();
  attenuation = 1.0 / (constant_ + linear_ * distance + quadratic_ * distance * distance);
  if (occlude)
	return ambient_ * attenuation * albedo;
  // calculate ambient coefficient
  ambient = ambient_;
  // calculate diffuse coefficient
  light_dir = light_dir.Normalize();
  cos_theta = std::max(0.0, light_dir.Dot(normal));
  diffuse = diffuse_ * cos_theta;
  // calculate specular coefficient
  view_dir = (view_pos - pos).Normalize();
  reflect_dir = Reflect(light_dir, normal).Normalize();
  cos_theta = std::max(0.0, view_dir.Dot(reflect_dir));
  specular = specular_ * std::pow(cos_theta, 32);
  // merge together
  return (ambient + diffuse + specular) * attenuation * albedo;
}

Vector4d SpotLight::Lighting(const Vector4d &normal,
							 const Vector4d &pos,
							 const Vector4d &view_pos,
							 const Vector4d &albedo,
							 bool occlude) {
  if (occlude)
	return Vector4d(0.0, 0.0, 0.0, 1.0);
  double cos_theta, intensity;
  Vector4d diffuse, specular, view_dir, reflect_dir, light_dir;
  // Determine if the viewpoint is inside the cone
  light_dir = (light_pos_ - pos).Normalize();
  cos_theta = light_dir.Dot(spot_dir_);
  intensity = (cos_theta - outer_cutoff_) / (inner_cutoff_ - outer_cutoff_);
  Clamp(intensity, 0.0, 1.0);
  // calculate diffuse coefficient
  cos_theta = std::max(0.0, light_dir.Dot(normal));
  diffuse = diffuse_ * cos_theta;
  // calculate specular coefficient
  view_dir = (view_pos - pos).Normalize();
  reflect_dir = Reflect(light_dir, normal).Normalize();
  cos_theta = std::max(0.0, view_dir.Dot(reflect_dir));
  specular = specular_ * std::pow(cos_theta, 32);
  // merge together
  return (diffuse + specular) * intensity * albedo;
}

Vector4d DirectionLight::PBRLighting(const Vector4d &normal,
									 const Vector4d &pos,
									 const Vector4d &view_pos,
									 const Vector4d &albedo,
									 bool occlude) {
  Vector4d view_dir = (view_pos - pos).Normalize();
  Vector4d half_vec = (view_dir + light_dir_).Normalize();
  Vector4d f0(0.04, 0.04, 0.04, 0.0);
  double roughness = 0.0;
  double cos_theta_i = std::max(light_dir_.Dot(normal), 0.0);
  double cos_theta_o = std::max(view_dir.Dot(normal), 0.0);

  Vector4d f = FE(f0, half_vec, view_dir);
  double d = NDF(normal, half_vec, roughness);
  double g = GE(normal, view_dir, light_dir_, roughness);

  Vector4d vec(1.0, 1.0, 1.0, 0.0);
  Vector4d diffuse = (vec - f) * (albedo / kPI);
  Vector4d specular = (d * f * g) / (4.0 * cos_theta_i * cos_theta_o + 0.001);

  return (diffuse + specular) * light_color_ * cos_theta_i;
}

Vector4d PointLight::PBRLighting(const Vector4d &normal,
								 const Vector4d &pos,
								 const Vector4d &view_pos,
								 const Vector4d &albedo,
								 bool occlude) {
  Vector4d light_dir = (light_pos_ - pos).Normalize();
  Vector4d view_dir = (view_pos - pos).Normalize();
  Vector4d half_vec = (view_dir + light_dir).Normalize();
  Vector4d f0(0.04, 0.04, 0.04, 0.0);
  double roughness = 0.0;
  double cos_theta_i = std::max(light_dir.Dot(normal), 0.0);
  double cos_theta_o = std::max(view_dir.Dot(normal), 0.0);

  Vector4d f = FE(f0, half_vec, view_dir);
  double d = NDF(normal, half_vec, roughness);
  double g = GE(normal, view_dir, light_dir, roughness);

  Vector4d vec(1.0, 1.0, 1.0, 0.0);
  Vector4d diffuse = (vec - f) * (albedo / kPI);
  Vector4d specular = (d * f * g) / (4.0 * cos_theta_i * cos_theta_o + 0.001);

  return (diffuse + specular) * light_color_ * cos_theta_i;
}

Vector4d SpotLight::PBRLighting(const Vector4d &normal,
								const Vector4d &pos,
								const Vector4d &view_pos,
								const Vector4d &albedo,
								bool occlude) {
  return Vector4d{};
}