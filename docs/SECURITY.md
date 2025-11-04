# Security Best Practices for VibraGuard

This document outlines security best practices for developing and deploying the VibraGuard system.

## Configuration Security

### Environment Variables

**NEVER commit sensitive credentials to version control!**

All sensitive configuration values should be stored in environment variables:

#### Backend Configuration
- Database credentials (`DB_USERNAME`, `DB_PASSWORD`)
- Twilio API credentials (`TWILIO_ACCOUNT_SID`, `TWILIO_AUTH_TOKEN`)
- Phone numbers (`MY_PHONE_NUMBER`, `FRIEND_PHONE_NUMBER`)
- Server URLs and endpoints

#### How to Use Environment Variables

**Local Development:**
1. Copy `.env.example` to `.env`
2. Fill in your actual values
3. The `.env` file is already in `.gitignore` and will not be committed

**Production Deployment:**
- Use your cloud provider's secrets management:
  - Google Cloud: Secret Manager
  - AWS: Secrets Manager or Parameter Store
  - Azure: Key Vault
- Set environment variables in your deployment configuration

### Database Security

✅ **DO:**
- Use environment variables for credentials
- Use strong, randomly generated passwords
- Limit database user permissions to only what's needed
- Use SSL/TLS for database connections in production
- Regularly update PostgreSQL to patch security vulnerabilities

❌ **DON'T:**
- Hardcode passwords in `application.properties`
- Use default passwords like "mypassword" in production
- Grant superuser access to application database users
- Expose database ports to the public internet

### MQTT Security

✅ **DO:**
- Use authentication for MQTT broker
- Implement ACLs (Access Control Lists) to restrict topic access
- Use TLS/SSL for MQTT connections in production
- Monitor MQTT traffic for suspicious activity

❌ **DON'T:**
- Leave MQTT broker without authentication
- Allow anonymous connections in production
- Use unencrypted MQTT in production environments

### API Security

✅ **DO:**
- Implement rate limiting on API endpoints
- Use HTTPS in production
- Validate and sanitize all input data
- Implement proper CORS policies
- Add authentication/authorization for sensitive endpoints

❌ **DON'T:**
- Expose admin endpoints without authentication
- Use HTTP in production
- Trust client-side data without validation
- Allow unlimited API requests (prone to DoS attacks)

### Resource Management

✅ **DO:**
- Always close database connections
- Properly clean up MQTT clients in finally blocks
- Use connection pooling for efficiency
- Monitor resource usage and set limits

❌ **DON'T:**
- Leave connections open indefinitely
- Create new connections without cleanup
- Ignore resource leaks

## Code Security Fixes Applied

This PR includes the following security improvements:

1. **Replaced hardcoded credentials with environment variables** in `application.properties`
2. **Fixed MQTT client resource leaks** - proper cleanup in finally blocks
3. **Removed hardcoded phone numbers** from configuration files
4. **Added environment variable support** to all test scripts
5. **Created `.env.example`** template for safe credential management

## Dependency Security

### Checking for Vulnerabilities

Run security checks regularly:

```bash
# Backend (Maven)
cd backend
mvn dependency-check:check

# Frontend (npm)
cd frontend
npm audit
npm audit fix  # Fix automatic issues
```

### Keeping Dependencies Updated

```bash
# Backend
mvn versions:display-dependency-updates

# Frontend
npm outdated
npm update
```

## Testing Security

- Never commit test credentials
- Use mock services for testing external APIs
- Rotate API keys regularly
- Use separate credentials for test environments

## Incident Response

If credentials are accidentally committed:

1. **Immediately rotate** all exposed credentials
2. Remove from git history using `git filter-branch` or BFG Repo-Cleaner
3. Force push to remote repository
4. Notify team members to pull latest changes
5. Review access logs for unauthorized usage

## Security Checklist for Deployment

- [ ] All sensitive data moved to environment variables
- [ ] `.env` files excluded from version control
- [ ] Database using strong passwords
- [ ] HTTPS/TLS enabled for all connections
- [ ] MQTT broker authentication enabled
- [ ] Firewall rules properly configured
- [ ] Regular security updates scheduled
- [ ] Monitoring and alerting configured
- [ ] Backup and disaster recovery plan in place

## Reporting Security Issues

If you discover a security vulnerability, please:
1. **DO NOT** open a public GitHub issue
2. Email the security team directly
3. Provide details about the vulnerability
4. Allow time for a fix before public disclosure

## References

- [OWASP Top 10](https://owasp.org/www-project-top-ten/)
- [Spring Security Best Practices](https://spring.io/projects/spring-security)
- [MQTT Security Fundamentals](https://www.hivemq.com/blog/mqtt-security-fundamentals/)
- [PostgreSQL Security](https://www.postgresql.org/docs/current/security.html)
