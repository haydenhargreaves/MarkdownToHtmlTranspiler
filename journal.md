Date: 2025/05/22
Desc: Rolling your own version control is not as hard as it sounds. This step by step guide will take you from 0 to 60!

# Self Hosted Git Server: How to

![Gitea Logo](https://hhargreaves.net/journal/gitea-logo.png)

<br>

###### Author: Hayden Hargreaves
###### Published: 05/22/2025

## Background

Version control is one of the most powerful tools used by developers, and Git is the most widely adopted **version control system** (vcs). However, when it comes to hosting Git, everyone does it a 
little differently. Most people use **[GitHub](https://github.com)** or even [GitLab](https://about.gitlab.com). Large companies typically host their
own for an added layer of safety and security. That is exactly what this guide will cover, but on a smaller
scale of course!

Before we dig into the details, what exactly does it mean to *"roll your own version control"* or *"host
your own git server"*? Well, it's simple, we are going to use a server of our own to deploy an application
that serves as a web-UI and *hub* for our Git repositories. Before you freak out, we are not going to 
actually write any code or build the application, there are countless open-source options available for 
**free** that "home-labbers" such as myself. In this guide, we will be using [Gitea](https://about.gitea.com) due to its ease of use
and strong support. 

*NOTE: As an added benefit, it was written in Go and is accepting contributions!*

## Requirements

There are only a few things you will need to roll your own Git server. The most important is a server, duh!
This can be a virtual private server (VPS), an EC2 instance from AWS, or your own hardware. Whatever you have 
will work, but my recommendation is to purchase your own hardware. I have a large server built of old gaming 
PC parts, but even a simple [Raspberry Pi](https://www.raspberrypi.com) will due! 

Once you have a server and root access (you will need to create and modify a user) you are about 99% there!
I assume that because you are reading this you have a personal computer. You will need SSH access to your 
server via a personal computer. This article will walk you through using **[Ansible](https://docs.ansible.com)** to configure your
server (which requires SSH access). **This guide assumes you are using a Debian or Ubuntu based Linux distro.**

Finally, the last "requirement" is optional, but highly recommended: a personal domain and a [Cloudflare](https://www.cloudflare.com) 
account. Regardless of whether you have a domain or not, you will be able to access your Git server from 
your local network. But, if you want access remotely securely, it is best to get your hands on a domain.
Using Cloudflare allows us access to their [tunnels](https://developers.cloudflare.com/cloudflare-one/connections/connect-networks/get-started/) which will allow us to expose local ports safely.
More details regarding these tunnels will come later.

*NOTE: There are other ways to access your server remotely without Cloudflare tunnels, but I will not cover that here.*

## Preview

Before continuing, please make sure you have everything you need to get started. Following these steps, 
**in order** will allow you to go from 0 to self hosting your server with relative ease!

1. **Install docker-compose:** We will be running the server in a docker container
2. **Create the *git* user:** Creating a new user will allow you to access the server using the git user
3. **Configure docker-compose:** This is the easiest way to install Gitea
4. **Configure the server:** The server can be configured via the web UI
5. **Configure SSH access:** The magic begins to happen here
6. **Configure remote access:** This is the final step that ties the bow on the whole system


### Disclaimer

It is assumed that you already have a basic understanding of Ansible and have a basic config setup. As this
is not an Ansible guide, I will not go into much detail there. However, many of these commands are easy to 
understand and can be used as normal shell commands.

For those who have ansible already configured on their system, we will be using the common **roles** pattern for
directories and files. A directory structure that looks something like this will yield the best results: 

```bash

.
├── ansible.cfg
├── inventory
│   ├── group_vars
│   │   └── main.yml
│   ├── hosts.yml
│   └── host_vars
│       └── gophernest.yml
├── playbooks
│   ├── common_setup.yml
│   └── docker_apps.yml
├── requirements.yml
└── roles
    ├── cloudflared
    │   ├── files
    │   │   ├── 3c522d3a-5f24-4645-b4ca-695c66e05ef3.json
    │   │   ├── cert.pem
    │   │   └── cloudflared
    │   ├── handlers
    │   │   └── main.yml
    │   ├── tasks
    │   │   └── main.yml
    │   ├── templates
    │   │   └── config.yml.j2
    │   └── vars
    │       └── main.yml
    ├── docker
    │   ├── handlers
    │   │   └── main.yml
    │   ├── tasks
    │   │   └── main.yml
    │   └── vars
    │       └── main.yml
    └── git
        ├── README.md
        ├── tasks
        │   └── main.yml
        ├── templates
        │   └── docker-compose.yml.j2
        └── vars
            └── main.yml
```

File paths will be provided at each step, if you are following along, you can use the structure above to create
an exact copy. **RECOMMENDED!**

<br>

## Install Docker Compose

The first requirement is to ensure that docker compose is installed. This can be done by updating the 
`roles/docker/tasks/main.yml` file to contain the following task.

```yaml
# roles/docker/tasks/main.yml

...

- name: Install Docker Compose
  get_url:
    url: https://github.com/docker/compose/releases/latest/download/docker-compose-linux-x86_64 # Modify system accordingly
    dest: /usr/local/bin/docker-compose
    mode: '0755'
  become: true
  tags:
    - docker
    - compose
```

Also, make sure you have a working installation of Docker on your system. Those not using Ansible can reference
the [docs](https://docs.docker.com/compose/install/) which provide a distro-specific installation guide.

You can test that this has worked successfully by running the docker compose command:

```bash
docker-compose --version
```

<br>

## Create the Git User

Now its time to create the user that will handle the server and manage the data. It is best practice to create 
a new user with permission only for this application, to follow the [principal of least privilege](https://en.wikipedia.org/wiki/Principle_of_least_privilege). This can 
be done very easily by updating the `roles/git/tasks/main.yml` file to contain the following tasks:

```yaml
# roles/git/tasks/main.yml

...

- name: Create git user
  user:
    name: git
    password: "{{ GIT_USER_PASSWORD }}"
    shell: /bin/bash
    state: present
  become: true
  tags:
    - git
    - user

- name: Add git user to the required groups
  user:
    name: git
    groups: sudo,docker
    append: yes
    state: present
  become: true
  tags:
    - git
    - groups
```

The password can be set directly here, or you can update the `roles/git/vars/main.yml` file to contain an entry
for the password. Ansible knows to look here when we use the syntax provided above.

```yaml
# roles/git/vars/main.yml

...

GIT_USER_PASSWORD: "super secret password" # use `mkpasswd -m sha-512 'password'`
```

For non Ansible users, this can be done with the typical Linux commands:

```bash
useradd -m -s /bin/bash git
passwd git

usermod -aG sudo git
usermod -aG docker git
```

<br>

## Configure Docker Compose
We will now create the required docker-compose file to start the application. The file should be placed in the 
new *git* users home directory, `/home/git/docker-compose.yml`. This can be done with a single task in the same
playbook as previous.

```yaml
# roles/git/tasks/main.yml

...

- name: Copy docker-compose file to the server
  template:
    src: docker-compose.yml.j2
    dest: /home/git/docker-compose.yml
    owner: git
    group: git
    mode: "0644"
  become: true
  tags:
    - git
    - docker
```

In order for this to work, we must also provide the `docker-compose.yml.j2` file in the `templates` directory.

```yaml
# roles/git/templates/docker-compose.yml.j2

networks:
  gitea:
    external: false

services:
  server:
    image: docker.gitea.com/gitea:1.23.8
    container_name: gitea
    environment:
      - USER=git
      - USER_UID=1001 # As the git user, run `id` to get UID and GID values
      - USER_GID=1002
    restart: always   # Allows the container to start when the server boots
    networks:
      - gitea
    volumes:
      - ./gitea:/data
      - /etc/timezone:/etc/timezone:ro
      - /etc/localtime:/etc/localtime:ro
    ports:
      - "4000:3000"
      - "222:22"      # Adjust the host ports as necessary, host:container
```

To do this manually, simply create a file `/home/git/docker-compose.yml` with the content in the above template.

## Configure the Server

We will use the Gitea web-UI to configure the server, but first we must start the server. With ansible, we can 
create the following task in the same location as the previous tasks (starting to notice a trend I hope).

```yaml
# roles/git/tasks/main.yml

...

- name: Start Docker compose application
  community.docker.docker_compose_v2:
    files: /home/git/docker-compose.yml
    project_src: /home/git
    state: present
    pull: always
  become: true
  tags:
    - git
    - start
```

Or you can run the docker compose command from the git users home directory `/home/git`:

```bash
docker-compose up -d # Use -d if you want it to run in the background, as a daemon
```

Now you can access our server locally using the local address of your server on port 4000 (or whatever you set 
in the docker compose file). For example, `http://192.168.1.2:4000`. You should see a configuration wizard, if 
so, you are almost done!

Feel free to customize these settings as you see fit, but ensure you follow the provided directions.

- Do not change the port's, HTTP or SSH, these are internal ports! To change the external ports, update the hosts
ports in the docker container.
- Leave the user as git, we set this up for a reason!
- Disable the **self registration** toggle in the advanced settings section (at the bottom).

<br>

## Configure Local Access

Your Git server is live! You have made it through the hardest part, the rest is easy. Access your server via HTTP
works but it's not the best but it works. So, now we will configure our local system to use [SSH key authentication](https://www.digitalocean.com/community/tutorials/how-to-configure-ssh-key-based-authentication-on-a-linux-server).
First you will need an SSH key, but I will leave that up to you to figure out.

Once you have your key, you need to add it to your Gitea server. The process is very similar to added an SSH key to 
GitHub, **Settings > SSH/GPG Keys > Add Key**. Then paste the content of your `*.pub` file into the content field.

Finally, we need to configure our local machine to use this key when we access our Git server. Update your `.gitconfig`
file to contain an entry similar to this:

```sshconfig
Host gitea # Update as needed
  Port 222 # Update as needed
  User git
  HostName 192.168.1.2 # Use your address here, we will change this later
  IdentityFile ~/.ssh/key
```

Much of these details will change when we setup our server to run on our domain, but for now, give them a try 
and adjust them accordingly.

When you attempt to clone a repo (for example) you will use the URL: 

```bash
git clone git@gitea:<username>/<repo>.git
```

Notice, we use **gitea** here as the host. Since this is how we configured our config to route to our server.

<br>

#### Side Note: Local Access

If you would only like access to this server from your local network then you can stop at this step. 

<br>

## Configure Remote Access

We will be using an existing **Cloudflare tunnel**, but I will not go into detail about setting one 
up. It is a pretty simple process that can be done without too much explanation. So, I will assume
you have a tunnel up and running. All we have to do, is route an endpoint from our local machine
to sub domain in our Cloudflare tunnel. By now, this should be easy for you, since you have setup
and configured your tunnel already (hopefully). But to remind you, you must add a record to your 
`config.yml` file, wherever it is on your system.

```yaml
...

ingress:
    - hostname: git.domain.net       # Enter your domain here
      service: http://localhost:4000 # Update the port as needed
    ...
```

But that is not all, the last step you need to do is add a [CNAME record](https://en.wikipedia.org/wiki/CNAME_record) in your Cloudflare 
DNS dashboard. This can be done manually, like you have before, or by creating an Ansible task
as follows. This will be its own role, `cloudflared`

```yaml
# roles/cloudflared/tasks/main.yml

...
# Update domain to your own
    
- name: Configure cloudflare Tunnel DNS Record (CNAMEs) for *.domain.net
  community.general.cloudflare_dns:
    zone: "domain.net"
    record: "{{ item }}.domain.net"
    type: "CNAME"
    value: "{{ tunnel_id }}.cfargotunnel.com"
    state: present
    proxied: true
    api_token: "{{ cloudflare_api_key }}"
  loop: "{{ domain_cnames }}"
  tags:
    - cloudflared
    - cnames
```

Like in the previous steps, we will need some variables in our `roles/cloudflared/vars/main.yml` file.

```yaml

...

tunnel_id: "tunnel_id"        # Enter your tunnel id here
cloudflare_api_key: "api_key" # Enter your API key here

# Include as many sub domains as you want, for now, we just need git
gophernest_cnames:
  - git               
  - ...
```

You may notice, we are using an API key. This is a free and simple process which is described [here](https://developers.cloudflare.com/fundamentals/api/get-started/create-token/)
in the Cloudflare docs. The key will allow us to update DNS records using their API.

Now, you will be able to access the web interface of your git server at *git.yourdomain.net*! However, 
we cannot use this domain with SSH to complete actions, such as cloning. At this state, you can clone
(or do other actions) using a URL that looks like this:

```bash
git clone git@your_servers_ip:username/repo.git
```

*NOTE: Tunneling TCP or UDP is more complex and will not be apart of this guide.*

But this is not ideal, nobody wants to use their server IP address to access their git server! So, what
can we do? Well, the best option is to simply use a DNS the *old* way. In your Cloudflare DNS panel, 
create an **A** entry with a value of whatever subdomain you want (we will need this later) and
the content being your servers IP address. For this record, make sure to *deselect* the proxied 
check box. What this will do is route the traffic from **subdomain.domain.net** to the IP address.

But why do we need that? Having a route will allow us to configure our SSH config to use this URL and 
access our git server via SSH without much effort. Update the previous record we created in our `~/.ssh/config` 
file to look more like this:

```sshconfig
Host gitea # Remeber this value!
  Port 222
  User git
  HostName subdomain.domain.net # This is the only change
  IdentityFile ~/.ssh/key
```

You should now be able to complete SSH actions using the **gitea** domain! An example would look like this:

```bash
git clone git@gitea:username/repo.git
```

Simple right! We are just about done, the last thing we need to do is update our Gitea config to use this 
new route in the frontend. You may notice that your web UI will provide a different value when you press 
clone on a repo (for example). To fix this, all you need to do edit your config file at 
`/home/git/gitea/gitea/conf/app.ini`. You will replace the line starting with `SSH_DOMAIN` to match whatever
value you labeled your SSH key to use.

For example:

```ini
[server]
...
SSH_DOMAIN = gitea
```

You may also edit any other domain values you see to match your own domain. These values will update the 
text fields that are provided to the user when actions are taken. After restarting the docker compose image
you will see the updates live!

For those using Ansible, this config change can be done using a simple task added to your `roles/git/tasks/main.yml`
file.

```yaml
# roles/git/tasks/main.yml

...

- name: Update the ssh domain in the config file if it exists
  replace:
    path: /home/git/gitea/gitea/conf/app.ini
    regexp: '^SSH_DOMAIN = (.*)'
    replace: 'SSH_DOMAIN = gitea'
  become: true
  tags:
    - git
    - config
```

*NOTE: I have also found it helpful to append this new task to the bottom of the **git** role as a safety measure.*

```yaml
# roles/git/tasks/main.yml

...

- name: Restart Docker compose application
  community.docker.docker_compose_v2:
    files: /home/git/docker-compose.yml
    project_src: /home/git
    state: restarted
    pull: always
  become: true
  tags:
    - git
    - restart
```

This will ensure the application is in its most recent state after each update.


## Conclusion

You now have your own version control server running in your home server! This solution should not replace 
GitHub in your workflow, some projects belong in the public eye. Your favorite projects are a great way for
future employers to see what kind of things you can do! But some things, like your Ansible config files, or 
your NixOS configuration, does not need to be public. Your home git server is a great place for those projects!
Just remember to make them private repos in Gitea ;)
