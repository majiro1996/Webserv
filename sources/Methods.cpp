/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmorales <moralesrojascr@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/29 10:40:55 by manujime          #+#    #+#             */
/*   Updated: 2024/04/10 19:15:08 by manujime         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Methods.hpp"

char **makeArgs (std::string exepath, std::string filepath)
{
    char **args = new char*[3];
    args[0] = strdup(exepath.c_str());
    args[1] = strdup(filepath.c_str());
    args[2] = NULL;
    return args;
}

Response Methods::HandleGet(std::string &path, Config &location)
{
    std::string body = "";
    std::string line;
    std::vector<Cgi> cgis = location.GetCgis();
    Response response;
    path = Utils::slashCleaner(path);
    //std::cout << "RRPATH: " << path << std::endl;
    if (Cgi::IsCgi(path))
    {
        //std::cout << "ENTRA1" << std::endl;
        Cgi cgi;
        std::cout << "CGIS SIZE: " << cgis.size() << std::endl;
        for (std::vector<Cgi>::iterator it = cgis.begin(); it != cgis.end(); it++)
        {
            std::cout << "EXTENSION: " << it->GetCgiExtension() << std::endl;
            std::cout << "PATH: " << it->GetCgiPath() << std::endl;
            if (it->GetCgiExtension() == path.substr(path.find_last_of('.')))
            {
                cgi = *it;
                break;
            }
        }
        if (cgi.GetCgiPath().empty())
        {
            std::cout << "NO CGI PATH" << std::endl;
            return Response(500);
        }
        char **args = makeArgs(Utils::slashCleaner(cgi.GetCgiPath()), Utils::slashCleaner(path));
        std::cout << "ARGS: " << args[0] << " " << args[1] << std::endl;
        if (cgi.ExecuteCgi(NULL, args, location.GetProjectPath()))
        {
            std::cout << "CGI RESULT: " << cgi.GetResult() << std::endl;
            response.setBody(cgi.GetResult());
            return response;
        }
        std::cout << "CGI ERROR" << std::endl; 
        return Response(500);
    }
    if(Utils::DirIsValid(path))
    {
        
        //std::cout << "DIRECTORIO" << path[path.size() - 1] << std::endl;
        if(location.GetAutoindex() == true)
        {
            body = AutoIndex::GetAutoIndex(path);
            response.setBody(body);
            return response;
        }
        if(path[path.size() - 1] == '/')
        {
            path += location.GetIndex();
        }
        else
        {
            path += "/";
            std::cout << location.GetIndex() << std::endl;
            std::cout << location.GetLocationName() << std::endl;
            path += location.GetIndex();
        }
    }
    std::ifstream file(path.c_str(), std::ios::binary);
    if (file.is_open())
    {
        
        //std::cout << "ENTRA2" << std::endl;
        std::ostringstream ss;
        ss << file.rdbuf();
        file.close();
        body = ss.str();
        response.setBody(body);
    }
    else
    {
        //std::cout << "ENTRA3" << std::endl;
        //response = "File not found";
        return Response(404);
    }
    return response;
}

Response Methods::HandleDelete(std::string path)
{
    Response response;

    std::ifstream file(path.c_str());
    if (!file.is_open())
        return Response(404);
    if (remove(path.c_str()) == 0)
        return response;
    return Response(500);

}

Response Methods::HandlePost(std::string path, std::string requestText, Config &config)
{
    Response response;
    std::string filename;
    std::string extension;
    size_t startPos = 0;
    size_t endPos = 0;

    while (true)
    {
        startPos = requestText.find("filename=\"", endPos);
        if (startPos == std::string::npos)
            break;
            
        startPos += 10;
        endPos = requestText.find("\"", startPos);
        if (endPos == std::string::npos)
            break;

        filename = requestText.substr(startPos, endPos - startPos);
        extension = filename.substr(filename.find_last_of('.'));
        if (filename.empty() || extension.empty())
            return Response(400); // Bad Request

        startPos = requestText.find("\r\n\r\n", endPos);
        if (startPos == std::string::npos)
            return Response(400);
        startPos += 4;
        endPos = requestText.find("\r\n------", startPos);
        if (endPos == std::string::npos)
            return Response(400);

        std::string fullPath = path + filename + extension;
        std::ofstream file(fullPath.c_str());
        if (!file.is_open())
            return Response(500);

        file << requestText.substr(startPos, endPos - startPos);
        file.close();
        endPos = requestText.find("\r\n------", endPos);
        if (endPos == std::string::npos || requestText.find("filename=\"", endPos) == std::string::npos)
            break;
    }

    return response;
    (void)config;
}