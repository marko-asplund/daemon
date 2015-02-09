/*
* daemon - http://libslack.org/daemon/
*
* Copyright (C) 1999-2001 raf <raf@raf.org>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
* or visit http://www.gnu.org/copyleft/gpl.html
*
* 20010215 raf <raf@raf.org>
*/

/*

=head1 NAME

I<daemon> - turns other processes into daemons

=head1 SYNOPSIS

 daemon [options] cmd arg...
 options:

   -h, --help                     - Print a help message then exit
   -V, --version                  - Print a version message then exit
   -v, --verbose[=level]          - Set the verbosity level
   -d, --debug[=level]            - Set the debug level

   -n, --name=name                - Name the client exclusively
   -u, --user=user[.group]        - Run the client as user[.group]
   -r, --respawn                  - Respawn the client
   -c, --core                     - Allow core file generation
   -s, --syslog=facility.priority - Send client's output to syslog
   -l, --log=facility.priority    - Send daemon's output to syslog
   -C, --config=path              - Specify the configuration file

=head1 DESCRIPTION

I<Daemon> turns other processes into daemons. There are many tasks that need
to be performed to correctly set up a daemon process. This can be tedious.
I<Daemon> performs these tasks for other processes.

The preparatory tasks that I<daemon> performs for other processes are:

=over 4

=item *

Disable core file generation to prevent security holes in daemons run by
root (unless the C<--core> option is supplied).

=item *

If I<daemon> was not invoked by I<init(8)> or I<inetd(8)>:

=over 4

=item *

Background the process to lose process group leadership.

=item *

Start a new process session.

=item *

Under I<SVR4>, background the process again to lose process session
leadership. This prevents the process from ever gaining a controlling
terminal. This only happens when C<SVR4> is defined and
C<NO_EXTRA_SVR4_FORK> is not defined when I<libslack> is compiled.

=back

=item *

Change to the root directory so as not to hamper umounts.

=item *

Clear the umask to enable explicit file creation modes.

=item *

Close all open file descriptors. If I<daemon> was invoked by I<inetd(8)>,
C<stdin>, C<stdout> and C<stderr> are left open since they are open to a
socket.

=item *

Open C<stdin>, C<stdout> and C<stderr> to C</dev/null> in case something
requires them to be open. Of course, this is not done if I<daemon> was
invoked by I<inetd(8)>.

=item *

If the C<--name> option is supplied, create and lock a file containing the
process id of the I<daemon> process. The presence of this locked file
prevents two instances of a daemon with the same name from running at the
same time. The default location of the pidfile is C</var/run> on Linux and
C</etc> on Solaris for I<root> or I</tmp> for ordinary users.

=back

I<Daemon> then spawns the client command specified on its command line and
waits for it to terminate. If the C<--user> option is supplied, I<daemon>
changes the user id and group id of the client process. Only root can use
this option. If the C<--syslog> option is supplied, the client's standard
output and error are captured by I<daemon> and sent to the I<syslog>
destination specified in the C<--syslog> option.

When the client terminates, I<daemon> respawns it if the C<--respawn> option
is supplied and the client terminated successfully after at least 600
seconds. Otherwise I<daemon> terminates.

If I<daemon> receives a C<SIGTERM> signal, it propagates the signal to the
client and then terminates.

B<Note:> For security reasons, never install I<daemon> with setuid or setgid
privileges. It is unnecessary. If you do, I<daemon> will revert to the real
user and group for safety before doing anything else.

=head1 OPTIONS

=over 4

=item C<-h>, C<--help>

Display a help message and exit.

=item C<-V>, C<--version>

Display a version message and exit.

=item C<-v>I<[level]>, C<--verbose>I<[=level]>

Set the message verbosity level to I<level> (or 1 if I<level> is not
supplied). I<daemon> does not have any verbose messages so this has no
effect.

=item C<-d>I<[level]>, C<--debug>I<[=level]>

Set the debug message level to I<level> (or 1 if I<level> is not supplied).
Set to level 1 for a trace of all functions called. Set to level 2 for more
detail. Debug messages are sent to the I<syslog(3)> facility,
C<daemon.debug>.

=item C<-n=>I<name>, C<--name=>I<name>

Create and lock a pid file (C</var/run/>I<name>C<.pid>), ensuring that only
one daemon with the given I<name> is active at the same time.

=item C<-u=>I<user[.group]>, C<--user=>I<user[.group]>

Run the client as a different user (and group). This only works for root.

=item C<-r>, C<--respawn>

Respawn the client when it terminates successfully after at least 600
seconds.

=item C<-c>, C<--core>

Allow the client to create a core file. This should only be used for
debugging as it could lead to security holes in daemons run by root.

=item C<-s=>I<facility.priority> C<--syslog=>I<facility.priority>

Capture the client's standard output and error and send it to the syslog
destination specified by I<facility.priority>.

=item C<-l=>I<facility.priority> C<--log=>I<facility.priority>

Send I<daemon>'s standard output and error to the syslog destination
specified by I<facility.priority>. By default, they are sent to
C<daemon.err>.

=item C<-C=>I<path>, C<--config=>I<path>

Specify the configuration file to use. By default, C</etc/daemon.conf> is
the configuration file if it exists and is not group or world writable and
does not exist in a group or world writable directory. The configuration
file lets you predefine options that apply to all clients and to
specifically named clients.

=back

=head1 FILES

C</etc/daemon.conf> - define default options

Each line of the configuration file consists of a client name or C<`*'>,
followed by whitespace, followed by a comma separated list of options. Blank
lines and comments (C<`#'> to end of the line) are ignored. Lines may be
continued with a C<`\'> character at the end of the line.

For example:

    *       log=daemon.err,syslog=local0.err,respawn
    test1   syslog=local0.debug,debug=9,verbose=9,core
    test2   syslog=local0.debug,debug=9,verbose=9,core

The command line options are processed first to look for a C<--config>
option. If no C<--config> option is supplied, the default file,
C</etc/daemon.conf>, is used. If the configuration file contains any generic
(C<`*'>) entries, their options are applied in order of appearance. If the
C<--name> option is supplied and the configuration file contains any entries
with the given name, their options are then applied in order of appearance.
Finally, the command line options are applied again. This ensures that any
generic options apply to all clients by default. Client specific options
override generic options. Command line options override both.

=head1 BUGS

You can't specify one syslog destination for the client's standard output
and a different one for its standard error.

You can't use spaces in client names in the configuration file.

If you specify (in the configuration file) that all clients allow core file
generation, there is no way to countermand that for any client (without
using an alternative configuration file). So don't do that. The same applies
to respawning but that shouldn't be problem.

It is possible for the client process to obtain a controlling terminal under
I<BSD> (and even under I<SVR4> if C<SVR4> was not defined or
C<NO_EXTRA_SVR4_FORK> was defined when I<libslack> is compiled). If anything
calls I<open(2)> on a terminal device without the C<O_NOCTTY> flag, the
process doing so will obtain a controlling terminal and then be susceptible
to termination by a C<SIGHUP>.

=head1 MAILING LISTS

The following mailing lists exist for daemon related discussion:

=over 4

 daemon-announce@libslack.org - Announcements
 daemon-users@libslack.org    - User forum
 daemon-dev@libslack.org      - Development forum

=back

To subscribe to any of these mailing lists, send a mail message to
I<listname>C<-request@libslack.org> with C<subscribe> as the message body.
e.g.

 $ echo subscribe | mail daemon-announce-request@libslack.org
 $ echo subscribe | mail daemon-users-request@libslack.org
 $ echo subscribe | mail daemon-dev-request@libslack.org

Or you can send a mail message to C<majordomo@libslack.org> with
C<subscribe> I<listname> in the message body. This way, you can
subscribe to multiple lists at the same time.
e.g.

 $ mail majordomo@libslack.org
 subscribe daemon-announce
 subscribe daemon-users
 subscribe daemon-dev
 .

A digest version of each mailing list is also available. Subscribe to
digests as above but append C<-digest> to the listname.

=head1 SEE ALSO

L<libslack(3)|libslack(3)>,
L<daemon(3)|daemon(3)>,
L<init(8)|init(8)>,
L<inetd(8)|inetd(8)>

=head1 AUTHOR

20010215 raf <raf@raf.org>

=cut

*/

#ifndef _BSD_SOURCE
#define _BSD_SOURCE /* For setgroups(2) on Linux */
#endif

#include <slack/std.h>

#include <string.h>
#include <signal.h>
#include <time.h>

#include <pwd.h>
#include <grp.h>
#include <syslog.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <slack/prog.h>
#include <slack/daemon.h>
#include <slack/sig.h>
#include <slack/err.h>
#include <slack/mem.h>
#include <slack/msg.h>
#include <slack/list.h>
#include <slack/str.h>

/* Configuration file entries */

typedef struct Config Config;
struct Config
{
	char *name;
	List *options;
};

#ifndef RESPAWN_THRESHOLD
#define RESPAWN_THRESHOLD 600
#endif

#ifndef CONFIG_PATH
#define CONFIG_PATH "/etc/daemon.conf"
#endif

/* Global variables */

static struct
{
	int ac;            /* number of command line arguments */
	char **av;         /* the command line arguments */
	char **cmd;        /* command vector to execute */
	char *name;        /* the daemon's name to use for the locked pid file */
	uid_t uid;         /* run the client as this user */
	gid_t gid;         /* run the client as this group */
	int respawn;       /* respawn the client process when it dies? */
	int core;          /* do we allow core file generation? */
	int client_syslog; /* syslog facility and priority for client output */
	int daemon_syslog; /* syslog facility and priority for daemon output */
	int daemon_debug;  /* syslog facility and priority for daemon debug output */
	char *config;      /* name of the config file to use - /etc/daemon.conf */
	pid_t pid;         /* the pid of the client process to run as a daemon */
	int pipe[2];       /* pipe fds for syslog */
	time_t spawn_time; /* when did we last spawn the client? */
}
g =
{
	0,                      /* ac */
	null,                   /* av */
	null,                   /* cmd */
	null,                   /* name */
	0,                      /* uid */
	0,                      /* gid */
	0,                      /* respawn */
	0,                      /* core */
	0,                      /* client_syslog */
	LOG_DAEMON | LOG_ERR,   /* daemon_syslog */
	LOG_DAEMON | LOG_DEBUG, /* daemon_debug */
	CONFIG_PATH,            /* config */
	(pid_t)0,               /* pid */
	{ -1, -1 },             /* pipe */
	(time_t)0               /* spawn_time */
};

/*

C<void term(int signo)>

Registered as the C<SIGTERM> handler. Propagates the C<SIGTERM> signal to the
client process. Calls I<daemon_close()> to unlink the locked pid file
(if any) and then calls I<exit(2)>.

*/

static void term(int signo)
{
	debug((1, "term(signo = %d)", signo))

	if (g.pid != 0 && g.pid != -1 && g.pid != getpid())
	{
		debug((2, "kill(term) process %d", g.pid))

		if (kill(g.pid, SIGTERM) == -1)
			errorsys("failed to kill the client process (%d)", g.pid);
	}

	exit(0);
}

/*

C<int examine_child(pid_t pid)>

Wait for the child process specified by C<pid>. If I<waitpid()> fails, return
C<ACTION_NOTHING> to continue the run loop as normal. If interrupted by a
signal while waiting, re-raise the signal again and then return
C<ACTION_SIGNAL> so that all signals will be handled. If the child was
killed by a signal, I<exit()>. If we are respawning the client, return
C<ACTION_RESPAWN> so that the client will be respawned, otherwise exit.

*/

enum { ACTION_NOTHING, ACTION_SIGNALS, ACTION_RESPAWN };

static int examine_child(pid_t pid)
{
	int status;
	pid_t wpid;

	debug((1, "examine_child(pid = %d)", pid))
	debug((2, "waitpid(pid = %d)", g.pid))

	if ((wpid = waitpid(g.pid, &status, 0)) == -1 && errno != EINTR)
	{
		errorsys("waitpid(%d) failed", g.pid);
		return ACTION_NOTHING;
	}

	if (wpid == -1 && errno == EINTR)
	{
		debug((2, "waitpid(%d) interrupted by a signal. raise chld and handle signals again", g.pid))
		signal_raise(SIGCHLD);
		return ACTION_SIGNALS;
	}

	debug((2, "pid %d received sigchld for pid %d", getpid(), wpid))

	if (WIFSIGNALED(status))
	{
		debug((2, "child killed by signal %d, exiting", WTERMSIG(status)))
		exit(0);
	}
	else if (WIFEXITED(status)) /* must be */
		debug((2, "child exited with status %d", WEXITSTATUS(status)))
	else if (WIFSTOPPED(status)) /* can't be */
		debug((2, "child stopped with signal %d", WSTOPSIG(status)))
	else /* can't be */
		debug((2, "child died under myterious circumstances"))

	if (g.respawn)
	{
		debug((2, "about to respawn"))
		return ACTION_RESPAWN;
	}

	debug((2, "child terminated, exiting"))
	exit(0);
}

static void spawn_child(void);

/*

C<void chld(int signo)>

Registered as the C<SIGCHLD> handler. Call I<examine_child()> to wait for the
child process. If C<ACTION_SIGNALS> is returned, handle all signals again.
If C<ACTION_RESPAWN> is returned, spawn the client again.

*/

static void chld(int signo)
{
	debug((1, "chld(signo = %d)", signo))

	switch (examine_child(g.pid))
	{
		case ACTION_SIGNALS: signal_handle_all(); break;
		case ACTION_RESPAWN: spawn_child(); break;
	}
}

/*

C<void prepare_parent(void)>

Before forking, set the term and chld signal handlers, revert to the user's
real uid and gid (if different to their effective uid and gid) and then set
the desired uid and gid from the --user option if required.

*/

static void prepare_parent(void)
{
	debug((1, "prepare_parent()"))

	debug((2, "setting sigterm action"))

	if (signal_set_handler(SIGTERM, 0, term) == -1)
		fatalsys("failed to set sigterm action");

	debug((2, "setting sigchld action"))

	if (signal_set_handler(SIGCHLD, 0, chld) == -1)
		fatalsys("failed to set sigchld action");
}

/*

C<void spawn_child(void)>

Create pipe (if syslogging) and fork. Child restores default term and
chld signal handling. If syslogging, close pipe[read] and dup pipe[write]
to stdout and stderr. Finally, child execs g.cmd. If syslogging, parent
closes pipe[read].

*/

static void spawn_child(void)
{
	time_t spawn_time;

	debug((1, "spawn_child()"))

	if ((spawn_time = time(0)) == -1)
		fatalsys("failed to get the time");

	if (g.spawn_time)
	{
		debug((2, "checking if enough time has passed to allow respawning"))

		if (spawn_time < g.spawn_time)
			fatal("refusing to respawn: the clock's gone backwards");

		if (spawn_time - g.spawn_time <= RESPAWN_THRESHOLD)
			fatal("refusing to respawn too quickly");
	}

	g.spawn_time = spawn_time;

	if (g.client_syslog)
	{
		debug((2, "creating pipe"))

		if (g.pipe[0] != -1)
			close(g.pipe[0]), g.pipe[0] = -1;

		if (g.pipe[1] != -1)
			close(g.pipe[1]), g.pipe[1] = -1;

		if (pipe(g.pipe) == -1)
			errorsys("failed to create pipe for --syslog");
	}

	debug((2, "forking"))

	switch (g.pid = fork())
	{
		case -1:
			fatalsys("failed to fork");

		case 0:
		{
			debug((2, "child pid = %d", getpid()))

			debug((2, "child restoring sigterm/sigchld actions"))

			if (signal_set_handler(SIGTERM, 0, SIG_DFL) == -1)
				fatalsys("child failed to restore sigterm action, exiting");

			if (signal_set_handler(SIGCHLD, 0, SIG_DFL) == -1)
				fatalsys("child failed to restore sigchld action, exiting");

			if (g.client_syslog && g.pipe[1] != -1)
			{
				debug((2, "child close(pipe[read]) and dup2(pipe[write] to stdout and stderr)"))

				if (close(g.pipe[0]) == -1)
					errorsys("child failed to close pipe[read]");

				g.pipe[0] = -1;

				if (g.pipe[1] != STDOUT_FILENO && dup2(g.pipe[1], STDOUT_FILENO) == -1)
					errorsys("child failed to dup2(pipe[write], stdout)");

				if (g.pipe[1] != STDERR_FILENO && dup2(g.pipe[1], STDERR_FILENO) == -1)
					errorsys("child failed to dup2(pipe[write], stderr)");

				if (g.pipe[1] != STDOUT_FILENO && g.pipe[1] != STDERR_FILENO)
					if (close(g.pipe[1]) == -1)
						errorsys("child failed to close(pipe[write]) after dup2()");
			}

			debug((1, "child execing %s", *g.cmd))
			execvp(*g.cmd, g.cmd);
			fatalsys("child failed to exec %s", *g.cmd);
		}
	}

	debug((2, "parent pid = %d\n", getpid()))

	if (g.client_syslog && g.pipe[1] != -1)
	{
		debug((2, "parent close(pipe[write])"))

		if (close(g.pipe[1]) == -1)
			errorsys("parent failed to close(pipe[write])");

		g.pipe[1] = -1;
	}
}

/*

C<void set_user()>

Clears supplementary groups and sets the process user and group to that
specified in the command line options. On success, returns 0. On error,
returns -1.

*/

static int set_user()
{
	debug((1, "set_user()"))

	debug((2, "setting user/group (if necessary)"))

	if (g.gid && (setgroups(0, null) == -1 || getgroups(0, null) != 0))
		return -1;

	if (g.gid && (setgid(g.gid) == -1 || getgid() != g.gid || getegid() != g.gid))
		return -1;

	if (g.uid && (setuid(g.uid) == -1 || getuid() != g.uid || geteuid() != g.uid))
		return -1;

	return 0;
}

/*

C<void run(void)>

The main run loop. Calls I<prepare_parent()> and I<spawn_child()>. If we're
sending the client's stdout and stderr to syslog, read from the pipe and
send anything rea dto I<syslog(3)>. Handle any signals that arrive in the
meantime. When there is no more to read from the pipe (either the client has
died or it has closed stdout and stderr), just I<waitpid(2)> for the client
to terminate. If we're not sending the client's stdout and stderr to syslog,
just <waitpid(2)> for the client to terminate.

*/

static void run(void)
{
	debug((1, "run()"))
	debug((2, "config: name %s, uid %d, gid %d, respawn %s, client syslog %s%s%s, daemon syslog %s%s%s, daemon debug %s%s%s, core %s, config %s, verbose %d, debug %d",
		g.name ? g.name : "<unnamed>",
		g.uid, g.gid,
		g.respawn ? "yes" : "no",
		g.client_syslog ? syslog_facility_str(g.client_syslog) : "",
		g.client_syslog ? "." : "None",
		g.client_syslog ? syslog_priority_str(g.client_syslog) : "",
		g.daemon_syslog ? syslog_facility_str(g.daemon_syslog) : "",
		g.daemon_syslog ? "." : "None",
		g.daemon_syslog ? syslog_priority_str(g.daemon_syslog) : "",
		g.daemon_debug ? syslog_facility_str(g.daemon_debug) : "",
		g.daemon_debug ? "." : "None",
		g.daemon_debug ? syslog_priority_str(g.daemon_debug) : "",
		g.core ? "yes" : "no",
		g.config,
		prog_verbosity_level(),
		prog_debug_level()
	))

	g.pipe[0] = g.pipe[1] = -1;
	prepare_parent();
	spawn_child();

	for (;;)
	{
		debug((2, "run loop - handle any signals"))
		signal_handle_all();

		/* Signals arriving here are lost */

		if (g.client_syslog && g.pipe[0] != -1)
		{
			char buf[BUFSIZ + 1];
			int n;

			debug((2, "syslogging, read pipe and wait for child"))

			while ((n = read(g.pipe[0], buf, BUFSIZ)) != -1 && n != 0)
			{
				char *p, *q;

				debug((2, "read(pipe[read]) returned %d", n))
				buf[n] = nul;

				for (p = buf; (q = strchr(p, '\n')); p = q + 1)
				{
					debug((2, "syslog(%*.*s)", q - p, q - p, p))
					syslog(g.client_syslog, "%*.*s", q - p, q - p, p);
				}

				if (*p && (*p != '\n' || p[1] != nul))
				{
					debug((2, "syslog(%s)", p))
					syslog(g.client_syslog, "%s", p);
				}
			}

			if (n == -1 && errno == EINTR)
			{
				debug((2, "read(pipe[read]) was interrupted by a signal\n"))
				continue;
			}

			if (n == -1)
			{
				errorsys("read(pipe[read]) failed");
				g.client_syslog = 0;
			}

			debug((2, "read() returned %d, closing pipe[read]", n))

			if (close(g.pipe[0]) == -1)
				errorsys("failed to close(pipe[read])");

			g.pipe[0] = -1;

			/*
			** Either the child has died and we have received/will receive
			** a SIGCHLD, or it has just closed stdout and stderr in which
			** case there's nothing more to redirect to syslog so we should
			** just wait for it to terminate by simulating a SIGCHLD. Even
			** if the child has already died, the SIGCHLD may not arrive for
			** some time but we can still waitpid() for it so we do by
			** simulating a SIGCHLD. If it the real SIGCHLD arrives later
			** and is handled, no harm is done.
			*/

			if (!signal_received(SIGCHLD))
			{
				signal_raise(SIGCHLD);
				debug((2, "simulating chld signal (chld cnt = %d)", signal_received(SIGCHLD)))
			}
		}
		else
		{
			debug((2, "not syslogging, just wait for child"))

			/* Assume SIGCHLD will arrive and wait for it */

			signal_raise(SIGCHLD);
			debug((2, "simulating chld signal (chld cnt = %d)\n", signal_received(SIGCHLD)))
		}
	}
}

/*

C<void store_syslog(const char *spec, int *var)>

Parse the syslog target C<spec> and store it in C<*var>.

*/

static void store_syslog(const char *spec, int *var)
{
	int facility;
	int priority;

	debug((1, "store_syslog(spec = %s)", spec))

	if (syslog_parse(spec, &facility, &priority) == -1)
		prog_usage_msg("Invalid syslog argument: failed to parse '%s'", spec);

	*var = facility | priority;
}

/*

C<void client_syslog(const char *spec)>

Parse and store the client syslog option argument, C<spec>.

*/

static void client_syslog(const char *spec)
{
	debug((1, "client_syslog(spec = %s)", spec))

	store_syslog(spec, &g.client_syslog);
}

/*

C<void daemon_syslog(const char *spec)>

Parse and store the daemon syslog option argument, C<spec>.

*/

static void daemon_syslog(const char *spec)
{
	debug((1, "daemon_syslog(spec = %s)", spec))

	store_syslog(spec, &g.daemon_syslog);
}

/*

C<void client_user(char *spec)>

Parse and store the client user[.group] option argument, C<spec>.

*/

static void client_user(char *spec)
{
	char *user, *group;
	struct passwd *pwd;
	struct group *grp;
	char **member;

	debug((1, "client_user(spec = %s)", spec))

	if (getuid() || geteuid())
		prog_usage_msg("The --user option only works for root");

	user = spec;
	group = strchr(spec, '.');
	if (group)
		*group++ = nul;

	pwd = getpwnam(user);
	if (!pwd)
		prog_usage_msg("Unknown user: '%s'", user);

	g.uid = pwd->pw_uid;
	g.gid = pwd->pw_gid;

	if (group)
	{
		grp = getgrnam(group);
		if (!grp)
			prog_usage_msg("Unknown group: '%s'", group);

		if (grp->gr_gid != pwd->pw_gid)
		{
			for (member = grp->gr_mem; *member; ++member)
				if (!strcmp(*member, user))
					break;

			if (!*member)
				prog_usage_msg("User %s is not in group %s", user, group);
		}

		g.gid = grp->gr_gid;
	}
}

/*

C<Config *config_create(char *name, char *options)>

Create a C<Config> object from a name and a comma separated list of C<options>.

*/

static Config *config_create(char *name, char *options)
{
	Config *config;
	char *tok;

	debug((1, "config_create(name = \"%s\", options = \"%s\")", name, options))

	if (!(config = mem_new(Config)))
		return null;

	if (!(config->name = mem_strdup(name)))
	{
		mem_release(config);
		return null;
	}

	if (!(config->options = list_create(free)))
	{
		mem_release(config->name);
		mem_release(config);
		return null;
	}

	for (tok = strtok(options, ","); tok; tok = strtok(tok + strlen(tok) + 1, ","))
	{
		size_t size = strlen(tok) + 3;
		char *option;

		if (!(option = mem_create(size, char)))
		{
			list_release(config->options);
			mem_release(config->name);
			mem_release(config);
			return null;
		}

		strlcpy(option, "--", size);
		strlcat(option, tok, size);

		if (!list_append(config->options, option))
		{
			mem_release(option);
			list_release(config->options);
			mem_release(config->name);
			mem_release(config);
			return null;
		}
	}

	return config;
}

/*

C<void config_release(Config *config)>

Release all memory associated with C<config>.

*/

static void config_release(Config *config)
{
	mem_release(config->name);
	list_release(config->options);
	mem_release(config);
}

/*

C<void config_parse(void *obj, const char *path, char *line, size_t lineno)>

Parse a C<line> of the configuration file, storing results in C<obj> which
is a list of lists of strings. C<lineno> is the current line number within
the configuration file.

*/

static void config_parse(void *obj, const char *path, char *line, size_t lineno)
{
	List *list = (List *)obj;
	Config *config;
	char name[128] = "";
	char options[1024] = "";

	debug((1, "config_parse(obj = %p, path = %s, line = \"%s\", lineno = %d)", obj, path, line, lineno))

	if (sscanf(line, "%127s %1023s", name, options) != 2)
		prog_usage_msg("config: syntax error in %s, line %d\n", path, lineno);

	if (!(config = config_create(name, options)) || !list_append(list, config))
		prog_usage_msg("config: out of memory");
}

/*

C<void config_process(List *conf, char *target)>

Searches for C<target> in C<conf> and processes the all configuration lines
that match C<target>.

*/

static void config_process(List *conf, char *target)
{
	int ac;
	char **av;
	Config *config;
	int j;

	while (list_has_next(conf))
	{
		config = (Config *)list_next(conf);

		if (!strcmp(config->name, target))
		{
			if (!(av = mem_create(list_length(config->options) + 2, char *)))
				prog_usage_msg("config: out of memory");

			av[0] = (char *)prog_name();

			for (j = 1; list_has_next(config->options); ++j)
				if (!(av[j] = mem_strdup(list_next(config->options))))
					prog_usage_msg("config: out of memory");

			av[ac = j] = null;
			optind = 0;
			prog_opt_process(ac, av);
			mem_release(av); /* Leak av elements since g might refer to them now */
		}
	}
}

/*

C<void config(void)>

Parse the configuration file, if any, and process the contents as command
line options. Generic options are applied to all clients. Options specific
to a particular named client override the generic options. Command line options
override both specific and generic options.

*/

static void config(void)
{
	List *conf;

	debug((1, "config()"))

	/* Check the the config file is safe */

	switch (daemon_path_is_safe(g.config))
	{
		case  0: error("Ignoring unsafe %s", g.config);
		case -1: return;
	}

	/* Parse the config file */

	if (!(conf = list_create((list_release_t *)config_release)))
		return;

	if (!daemon_parse_config(g.config, conf, config_parse))
	{
		list_release(conf);
		return;
	}

	/* Apply generic options */

	config_process(conf, "*");

	/* Override with specific options */

	if (g.name)
		config_process(conf, g.name);

	/* Override with command line options */

	optind = 0;
	prog_opt_process(g.ac, g.av);

	list_release(conf);
}

/*

C<Option daemon_optab[];>

Application specific command line options.

*/

static Option daemon_optab[] =
{
	{
		"name", 'n', "name", "Prevent multiple named instances",
		required_argument, OPT_STRING, OPT_VARIABLE, &g.name
	},
	{
		"user", 'u', "user[.group]", "Run the client as user[.group]",
		required_argument, OPT_STRING, OPT_FUNCTION, (void *)client_user
	},
	{
		"respawn", 'r', null, "Respawn the client",
		no_argument, OPT_NONE, OPT_VARIABLE, &g.respawn
	},
	{
		"core", 'c', null, "Allow core file generation",
		no_argument, OPT_NONE, OPT_VARIABLE, &g.core
	},
	{
		"syslog", 's', "facility.priority", "Send client's output to syslog",
		required_argument, OPT_STRING, OPT_FUNCTION, (void *)client_syslog
	},
	{
		"log", 'l', "facility.priority", "Send daemon's output to syslog",
		required_argument, OPT_STRING, OPT_FUNCTION, (void *)daemon_syslog
	},
	{
		"config", 'C', "path", "Specify the configuration file",
		required_argument, OPT_STRING, OPT_VARIABLE, &g.config
	},
	{
		null, nul, null, null, 0, 0, 0, null
	}
};

static Options options[1] = {{ prog_options_table, daemon_optab }};

/*

C<void id(void)>

Supplies program identification for use in help, version and error messages.

*/

static void id(void)
{
	prog_set_name(DAEMON_NAME);
	prog_set_version(DAEMON_VERSION);
	prog_set_date(DAEMON_DATE);
	prog_set_syntax("[options] cmd arg...");
	prog_set_options(options);
	prog_set_author("raf <raf@raf.org>");
	prog_set_contact(prog_author());
	prog_set_url(DAEMON_URL);
	prog_set_legal
	(
		"Copyright (C) 1999-2001 raf <raf@raf.org>\n"
		"\n"
		"This is free software released under the terms of the GPL:\n"
		"\n"
		"    http://www.gnu.org/copyleft/gpl.html\n"
		"\n"
		"There is no warranty; not even for merchantability or fitness\n"
		"for a particular purpose.\n"
#ifdef NEEDS_GETOPT
		"\n"
		"Includes the GNU getopt functions:\n"
		"    Copyright (C) 1997, 1998 Free Software Foundation, Inc.\n"
#endif
	);

	prog_set_desc
	(
		"Daemon turns other processes into daemons.\n"
		"See the daemon(1) manpage for more information.\n"
	);
}

/*

C<void init(int ac, char **av)>

Initialises the program. Revokes any setuid/setgid privileges. Processes
command line options. Processes the configuration file. Calls
I<daemon_prevent_core()> unless the C<--core> option is supplied. Calls
I<daemon_init()> with the C<--name> option's argument, if any. Arranges to
have C<SIGTERM> signals propagated to the client process. And stores the
remaining command line arguments to be I<execvp()>d later.

*/

static void init(int ac, char **av)
{
	int a;

	prog_init();
	id();

	if (daemon_revoke_privileges() == -1)
		fatalsys("failed to revoke uid/gid privileges: uid/gid = %d/%d euid/egid = %d/%d", getuid(), getgid(), geteuid(), getegid());

	if ((a = prog_opt_process(g.ac = ac, g.av = av)) == ac)
		prog_usage_msg("Invalid arguments: no command supplied");

	if (set_user() == -1)
	{
		struct group *grp = getgrgid(g.gid);
		struct passwd *pwd = getpwuid(g.uid);
		fatalsys("failed to set user/group exclusively to %s/%s (%d/%d): uid/gid = %d/%d euid/egid = %d/%d #groups = %d", (pwd) ? pwd->pw_name : "<noname>", (grp) ? grp->gr_name : "<noname>", g.uid, g.pid, getuid(), getgid(), geteuid(), getegid(), getgroups(0, null));
	}

	config();

	if (!g.core && daemon_prevent_core() == -1)
		fatalsys("failed to prevent core file generation");

	if (prog_dbg_syslog(null, 0, g.daemon_debug) == -1)
		fatalsys("failed to start debug delivery to %s.%s", syslog_facility_str(g.daemon_debug), syslog_priority_str(g.daemon_debug));

	if (prog_err_syslog(null, 0, g.daemon_syslog) == -1)
		fatalsys("failed to start error delivery to %s.%s", syslog_facility_str(g.daemon_syslog), syslog_priority_str(g.daemon_syslog));

	if (!(g.cmd = mem_create(ac - a + 1, char *)))
		prog_usage_msg("out of memory");

	memmove(g.cmd, av + a, (ac - a) * sizeof(char *));
	g.cmd[ac - a] = null;

	if (daemon_init(g.name) == -1)
		fatalsys("failed to start daemon");

	if (g.name && atexit((void (*)(void))daemon_close) == -1)
		fatalsys("failed to atexit(daemon_close)");
}

/*

C<int main(int ac, char **av)>

Initialise the program with the command line arguments specified in
C<ac> and C<av> then run it.

*/

int main(int ac, char **av)
{
	init(ac, av);
	run();
	return 0; /* unreached */
}

/* vi:set ts=4 sw=4: */
