#include <stdio.h>
#include <cups/cups.h>

// The printer uri for printers defined locally
#define PRINTER_URI "ipp://localhost/printers/"

static void authenticate_job(const char *job_uri, const char * username, 
			     char **password) {
  // only ask for password if the password hadn't been set yet
  // we postpone requesting a password as long as possible
  if (*password == NULL) {
    *password = getpass("Password: ");
  }

  const char *auth_info[] = {username, *password};

  // create a new 'authenticate job' request'
  ipp_t *request = ippNewRequest(IPP_OP_CUPS_AUTHENTICATE_JOB);

  // add job-uri to the request
  ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI, "job-uri",
	       NULL, job_uri);
  printf("Authenticating %s\n", job_uri);
    
  // add authentication information to the request
  ippAddStrings(request, IPP_TAG_OPERATION, IPP_TAG_TEXT, "auth-info",
		2, NULL, auth_info);

  // the request is deleted by cupsDoRequest
  cupsDoRequest(CUPS_HTTP_DEFAULT, request, "/");
}

int authenticate_jobs(const char *username) {
  // create a 'get jobs' request
  ipp_t *request = ippNewRequest(IPP_GET_JOBS);

  // add the printer-uri to the request
  ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI, "printer-uri",
	       NULL, PRINTER_URI);

  // do a request, no authorization here, most likely stays on the same 
  // machine
  // the request is deleted by cupsDoRequest
  ipp_t *response = cupsDoRequest(CUPS_HTTP_DEFAULT, request, "/");

  if (response != NULL) {
    // walk over the response, if it is a job-uri, then authenticate the
    // job
    ipp_attribute_t *attr;
    const char *attrname;
    const char *job = NULL;
    char *password = NULL;

    for (attr = ippFirstAttribute(response); attr != NULL; 
	 attr = ippNextAttribute(response)) {

      attrname = ippGetName(attr);

      if (attrname != NULL && !strcmp(attrname, "job-uri")) {
	job = ippGetString(attr, 0, NULL);
	authenticate_job(job, username, &password);
      }
    }
  }
  else {
    return 1;
  }

  return 0;
}

