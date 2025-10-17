output "rds_endpoint" {
  value       = aws_db_instance.postgres.address
  description = "Hôte de connexion PostgreSQL"
}

output "rds_port" {
  value = aws_db_instance.postgres.port
}

output "rds_db_name" {
  value = aws_db_instance.postgres.db_name
}

output "connection_string" {
  sensitive = true
  value     = "postgres://${var.db_username}:${var.db_password}@${aws_db_instance.postgres.address}:${aws_db_instance.postgres.port}/${aws_db_instance.postgres.db_name}"
}
