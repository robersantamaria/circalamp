import functools
import logging.config
from time import gmtime, strftime

from flask import Flask, request, send_from_directory


app = Flask(__name__)

CONTENT_TYPE_JSON = {"Content-Type": "application/json"}
CONTENT_TYPE_TEXT = {"Content-Type": "text/plain"}


def log(fn):
    @functools.wraps(fn)
    def wrap(*args, **kwargs):
        logger.info("============\nEntering {}".format(fn.__name__))
        logger.info(str(request.headers).strip())
        logger.info("Payload: {}".format(request.get_data()))
        return fn(*args, **kwargs)

    return wrap


@app.route("/home")
@log
def index():
    return send_from_directory(".", "index.html")


@app.route("/lights", methods=["POST"])
@log
def lights():
    return "", 202


CURRENT_TIME = None
CURRENT_ALARM_TIME = None
CURRENT_ALARM_SWITCH = False


@app.route("/time", methods=["GET"])
@log
def get_time():
    global CURRENT_TIME
    if CURRENT_TIME:
        now = CURRENT_TIME
    else:
        now = strftime("%Y/%m/%dT%H:%M", gmtime())
    return now, 200, CONTENT_TYPE_TEXT


@app.route("/time", methods=["POST"])
@log
def set_time():
    global CURRENT_TIME
    CURRENT_TIME = request.form.get("time")
    return CURRENT_TIME, 200, CONTENT_TYPE_TEXT


@app.route("/alarm", methods=["GET"])
@log
def get_alarm():
    global CURRENT_ALARM_TIME
    global CURRENT_ALARM_SWITCH
    if CURRENT_ALARM_TIME:
        now = CURRENT_ALARM_TIME
    else:
        now = strftime("%Y/%m/%dT%H:%M", gmtime())
    alarm_switch = "on" if CURRENT_ALARM_SWITCH else "off"
    return now + " " + alarm_switch, 200, CONTENT_TYPE_TEXT


@app.route("/alarm", methods=["POST"])
@log
def set_alarm():
    global CURRENT_ALARM_TIME
    global CURRENT_ALARM_SWITCH
    CURRENT_ALARM_TIME = request.form.get("alarm_time")
    CURRENT_ALARM_SWITCH = request.form.get("alarm_switch") == "on"
    alarm_switch = "on" if CURRENT_ALARM_SWITCH else "off"
    return CURRENT_ALARM_TIME + " " + alarm_switch, 200, CONTENT_TYPE_TEXT


logging.config.dictConfig(
    {
        "version": 1,
        "handlers": {
            "console": {
                "class": "logging.StreamHandler",
                "level": "DEBUG",
                "stream": "ext://sys.stdout",
            }
        },
        "loggers": {"elasticbox": {"level": "INFO", "handlers": ["console"]}},
    }
)

logger = logging.getLogger("elasticbox")
