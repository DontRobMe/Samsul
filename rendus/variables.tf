variable "project_name" {
  type    = string
  default = "babyfoot"
}

variable "aws_region" {
  type    = string
  default = "eu-west-3" # Paris
}

variable "db_name" {
  type    = string
  default = "babyfoot"
}

variable "db_username" {
  type    = string
  default = "appuser"
}

variable "db_password" {
  type      = string
  sensitive = true
}
