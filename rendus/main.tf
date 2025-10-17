terraform {
  required_version = ">= 1.6.0"
  required_providers {
    aws = {
      source  = "hashicorp/aws"
      version = "~> 5.0"
    }
  }
}

provider "aws" {
  region = var.aws_region
}

# RDS PostgreSQL minimal (pas de VPC/Subnet/SG dans le code)
resource "aws_db_instance" "postgres" {
  identifier        = "${var.project_name}-pg"
  engine            = "postgres"
  instance_class    = "db.t3.micro"
  allocated_storage = 20
  storage_type      = "gp3"

  db_name  = var.db_name
  username = var.db_username
  password = var.db_password

  publicly_accessible = true
  skip_final_snapshot = true
  deletion_protection = false
  apply_immediately   = true
}
