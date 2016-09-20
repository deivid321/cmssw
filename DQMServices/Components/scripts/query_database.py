import netrc
import sys
import sqlite3
import argparse
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, CheckButtons
from sqlalchemy import *
from pylab import *
import cx_Oracle

def read_data(conn, values, path, run, luminosity, min, max):
    properties = []
    for i in range(0, 20):
        properties.append([])

    def set_properties(stm):
        rs = conn.execute(stm).fetchall()
        for row in rs:
            properties[0].append(row[0])
            properties[1].append(row[2])
            for i in range(2, 20):
                properties[i].append(row[i-2 + 4])

    try:
        if (path and min!=None and max!=None and run >= 0):
            print "QUERY: SELECT * FROM HISTOGRAM_VALUES WHERE LUMISECTION >=", min, " WHERE LUMISECTION <=", max, "AND RUN_NUMBER =", run, "AND PATH =", path
            stm = values.select(and_(values.c.PATH == path, values.c.LUMISECTION >= min, values.c.LUMISECTION <= max, values.c.RUN_NUMBER == run))
            set_properties(stm)
        elif (path and min!=None and max==None and run >= 0):
            print "QUERY: SELECT * FROM HISTOGRAM_VALUES WHERE LUMISECTION >=", min, "AND RUN_NUMBER =", run, "AND PATH =", path
            stm = values.select(and_(values.c.PATH == path, values.c.LUMISECTION >= min, values.c.RUN_NUMBER == run))
            set_properties(stm)
        elif (path and min==None and max!=None and run >= 0):
            print "QUERY: SELECT * FROM HISTOGRAM_VALUES WHERE LUMISECTION <=", max, "AND RUN_NUMBER =", run, "AND PATH =", path
            stm = values.select(and_(values.c.PATH == path, values.c.LUMISECTION <= max, values.c.RUN_NUMBER == run))
            set_properties(stm)
        elif (path and luminosity>=0 and run >= 0):
            print "QUERY: SELECT * FROM HISTOGRAM_VALUES WHERE LUMISECTION =", luminosity, "AND RUN_NUMBER =", run, "AND PATH =", path
            stm = values.select(and_(values.c.PATH == path, values.c.LUMISECTION == luminosity, values.c.RUN_NUMBER == run))
            set_properties(stm)
        elif (path and not luminosity and run != None):
            print "QUERY: SELECT * FROM HISTOGRAM_VALUES WHERE RUN_NUMBER =", run, "AND PATH =", path
            stm = values.select(and_(values.c.RUN_NUMBER == run, values.c.PATH == path))
            set_properties(stm)
        elif (path and luminosity and run == None):
            print "QUERY: SELECT * FROM HISTOGRAM_VALUES WHERE LUMISECTION =", luminosity, "AND PATH =", path
            stm = values.select(and_(values.c.PATH == path, values.c.LUMISECTION == luminosity))
            set_properties(stm)
        elif (path and not luminosity and run == None):
            print "QUERY: SELECT * FROM HISTOGRAM_VALUES WHERE PATH =", path
            stm = values.select(values.c.PATH == path)
            set_properties(stm)

        if (properties[0] == []):
            print "INFO: No results!"
            sys.exit(2)
        else:
            print "INFO: Properties: :" + properties.__str__() + '\n'
            return properties

    except e:
        print "ERROR: An error occurred:", e


def show_data(properties):
    property_name = ['PATH', 'LUMISECTION', 'X_MEAN', 'X_MEAN_ERROR', 'X_RMS', 'X_RMS_ERROR', 'X_UNDERFLOW',
                     'X_OVERFLOW', 'Y_MEAN', 'Y_MEAN_ERROR', 'Y_RMS', 'Y_RMS_ERROR', 'Y_UNDERFLOW','Y_OVERFLOW',
                     'Z_MEAN', 'Z_MEAN_ERROR', 'Z_RMS', 'Z_RMS_ERROR', 'Z_UNDERFLOW', 'Z_OVERFLOW']
    show_data.isLabeled = True

    fig = plt.figure(figsize=(14, 8))
    fig.canvas.set_window_title('Histogram values')
    ax = fig.add_subplot(111)
    plt.xticks(properties[1])
    x = properties[1]  #lumisections
    y = properties[2]  #x_mean
    nr = 18
    #In case we need to show only those values which exist
    #for prop in properties[2:]:
    #    if (prop[0]!=0.0):
    #        nr += 1
    plot1, = ax.plot( x, y, 'ro', picker=5)
    plt.axis([min(x) - 1, max(x) + 1, min(y) - 0.0001, max(y) + 0.0001])  # 0 values generate bottom==top error for axis, thats why +-0.0001

    read_data.ano = []
    for xy in zip(x, y):
        read_data.ano.append(ax.annotate(' (%s)' % xy[1], xy=xy, textcoords='data'))

    path = properties[0][0]
    plt.xlabel('Luminosity')
    plt.title(path + '\n' + property_name[2])
    plt.grid()

    subplots_adjust(bottom=0.25)
    axcolor = 'lightgoldenrodyellow'
    axfreq = axes([0.125, 0.1, 0.78, 0.03], axisbg=axcolor)
    sfreq = Slider(axfreq, 'Property', 1, nr, valinit=1, valfmt='%0.0f')
	
    def update(val):
        [ax.texts.remove(a) for a in read_data.ano]
        del read_data.ano[:]
        y = properties[int(round(val))+1]
        plot1.set_ydata(y)
        ax.set_title(path + '\n' + property_name[int(round(val)) + 1])
        ax.set_ylim([min(y) - 0.0001, max(y) + 0.0001])
        if show_data.isLabeled:
            for xy in zip(x, y):
                read_data.ano.append(ax.annotate(' (%s)' % xy[1], xy=xy, textcoords='data'))
        draw()

    sfreq.on_changed(update)

    def onpick(event):
        thisline = event.artist
        xdata = thisline.get_xdata()
        ydata = np.asarray(thisline.get_ydata())
        points = tuple(zip(xdata[event.ind], ydata[event.ind]))
        str = ' (%s)' % points[0][1]
        for s in read_data.ano:
            if s._text == str and s.xy[0] == points[0][0]:
                ax.texts.remove(s)
                read_data.ano.remove(s)
                draw()
                return
        read_data.ano.append(ax.annotate(str, xy=points[0], textcoords='data'))
        draw()

    fig.canvas.mpl_connect('pick_event', onpick)

    rax = plt.axes([0.905, 0.5, 0.085, 0.1])
    label = CheckButtons(rax, ('Label', ), (True,))

    def label_click(label):
        if show_data.isLabeled:
            show_data.isLabeled = False
            update(sfreq.val)
        else:
            show_data.isLabeled = True
            update(sfreq.val)

    label.on_clicked(label_click)

    show()


if __name__ == "__main__":
    database = ''
    default_database = 'sqlite:///db1.db'
    user = ''
    password = ''

    parser = argparse.ArgumentParser()
    parser.add_argument("-d", "--default", help="Use default database. Otherwise uses oracle with .netrc configuration", action='store_true')
    parser.add_argument("-r", "--run", help="specify run number for SQL query", type=int)
    parser.add_argument("-l", "--luminosity", help="specify luminosity number for SQL query", type=int)
    parser.add_argument("-min", "--minLuminosity", help="specify min luminosity number for SQL query", type=int)
    parser.add_argument("-max", "--maxLuminosity", help="specify max luminosity number for SQL query", type=int)
    parser.add_argument("-p", "--path", help="specify histogram path for SQL query, if you also specify run (but no luminosity), program will display plots of histogram properties vs luminosities")

    args = parser.parse_args()

    get = netrc.netrc()
    print(get.authenticators('oracleDB'))
    auth = get.authenticators('oracleDB')

    if (not args.path and not args.luminosity and args.run == None):
        parser.print_help()
        sys.exit(2)
    try:
        if args.default:
            eng = create_engine(default_database)
            conn = eng.connect()
            print "INFO: Connected to the default database"
        else:
            eng = create_engine("oracle://"+auth[0] +":"+auth[2]+"@cms_orcoff_prep", echo=True)
            conn = eng.connect()
            print "INFO: Connected to the oracle database: oracle://"+auth[0] +":"+auth[2]+"@cms_orcoff_prep"
    except sqlite3.Error as e:
        print "ERROR: An error occurred:", e.args[0]

    meta = MetaData(eng)
    values = Table('HISTOGRAM_VALUES', meta,
 	    Column('PATH', String, primary_key=True),
        Column('RUN_NUMBER', Integer, primary_key=True),
        Column('LUMISECTION', Integer, primary_key=True),
        Column('ENTRIES', Integer, primary_key=True),
        Column('X_MEAN', String, nullable=False),
	    Column('X_MEAN_ERROR', String, nullable=False),
	    Column('X_RMS', String, nullable=False),
	    Column('X_RMS_ERROR', String, nullable=False),
	    Column('X_UNDERFLOW', String, nullable=False),
	    Column('X_OVERFLOW', String, nullable=False),
	    Column('Y_MEAN', String, nullable=False),
	    Column('Y_MEAN_ERROR', String, nullable=False),
	    Column('Y_RMS', String, nullable=False),
	    Column('Y_RMS_ERROR', String, nullable=False),
	    Column('Y_UNDERFLOW', String, nullable=False),
	    Column('Y_OVERFLOW', String, nullable=False),
        Column('Z_MEAN', String, nullable=False),
        Column('Z_MEAN_ERROR', String, nullable=False),
        Column('Z_RMS', String, nullable=False),
        Column('Z_RMS_ERROR', String, nullable=False),
        Column('Z_UNDERFLOW', String, nullable=False),
        Column('Z_OVERFLOW', String, nullable=False))

    properties = read_data(conn, values, args.path, args.run, args.luminosity, args.minLuminosity, args.maxLuminosity)

    show_data(properties)
