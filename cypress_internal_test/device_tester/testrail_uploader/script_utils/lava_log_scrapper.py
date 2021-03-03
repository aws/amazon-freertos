"""
Implements request for Lava server to get logs by job id.
Uses v0.2 REST API.
Hermann Yavorskyi <hermann.yavorskyi@cypress.com>
"""
import requests

# API endpoint which is used for requsts
URL = "http://lb10-dispatch.ric.cypress.com/api/v0.2/jobs/"


class LavaGetLogException(Exception):
    """
    Class represents exception which can occure during handling
    the request to rest api with invalid job_id or other connection/
    server side issues.
    """


def get_lava_log_by_job_id(job_id):
    """
    Returns the logs` content of job with given job_id

    Args:
        job_id (int): Id of the job on Lava which logs are requested.

    Returns:
        (str): content of the log file.
    """
    request = requests.get(url=f"{URL}{job_id}/logs/")

    if request.status_code >= 400:
        raise LavaGetLogException(
            f"Error while getting logs for job {job_id}.\n"
            "Check your connection and job id."
            "You should also be connected to intranet.")
    if request.status_code >= 300:
        raise LavaGetLogException("Server side error occured. Response:\n"
                                  f"{request.json()}\n"
                                  f"{request.content}")

    return request.content.decode("utf-8")
