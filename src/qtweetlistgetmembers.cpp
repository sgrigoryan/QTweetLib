/* Copyright (c) 2010, Antonie Jovanoski
 *
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact e-mail: Antonie Jovanoski <minimoog77_at_gmail.com>
 */

#include <QNetworkRequest>
#include <QNetworkReply>
#include "qtweetlistgetmembers.h"
#include "qtweetuser.h"

QTweetListGetMembers::QTweetListGetMembers(QObject *parent) :
    QTweetNetBase(parent)
{
}

QTweetListGetMembers::QTweetListGetMembers(OAuthTwitter *oauthTwitter, QObject *parent) :
        QTweetNetBase(oauthTwitter, parent)
{
}

/*!
  \param user User id
  \param list List id
  \param cursor Breaks the results into pages.
                This is recommended for users who are following many users.
                Provide a value of -1 to begin paging.
                Provide values as returned in the signal nextCursor and prevCursor to page back and forth in the list.
  \param includeEntities When set to true tweet will include a node called "entities,"
  \param respType Response type json (default) or xml
 */
void QTweetListGetMembers::get(qint64 user,
                               qint64 list,
                               const QString &cursor,
                               bool includeEntities,
                               ResponseType respType)
{
    Q_ASSERT(oauthTwitter() != 0);

    QString urlString = QString("http://api.twitter.com/1/%1/%2/members.").arg(user).arg(list);

    QUrl url;

    if (respType == QTweetNetBase::JSON)
        url.setUrl(urlString + "json");
    else
        url.setUrl(urlString + "xml");

    if (!cursor.isEmpty())
        url.addQueryItem("cursor", cursor);

    if (includeEntities)
        url.addQueryItem("include_entities", "true");

    QNetworkRequest req(url);

    QByteArray oauthHeader = oauthTwitter()->generateAuthorizationHeader(url, OAuth::GET);
    req.setRawHeader(AUTH_HEADER, oauthHeader);

    QNetworkReply *reply = oauthTwitter()->networkAccessManager()->get(req);
    connect(reply, SIGNAL(finished()), this, SLOT(reply()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error()));
}

void QTweetListGetMembers::parsingJsonFinished(const QVariant &json, bool ok, const QString &errorMsg)
{
    if (ok) {
        QVariantMap respMap = json.toMap();

        QVariant userList = respMap["users"];

        QList<QTweetUser> users = variantToUserInfoList(userList);

        QString nextCursor = respMap["next_cursor_str"].toString();
        QString prevCursor = respMap["prev_cursor_str"].toString();

        emit parsedUsers(users, nextCursor, prevCursor);
    } else {
        qDebug() << "QTweetListGetMembers json parsing error: " << errorMsg;
    }
}

void QTweetListGetMembers::error()
{
    // ### TODO
}
